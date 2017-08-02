#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include "bmi160.h"
#include "bmm150.h"
#include "thread.h"

#ifdef USE_UDP
#include "udplink.h"
#endif

#define SPIDEV 		3
#define SPICS  		0
#define I2CDEV 		2
#define I2CSLAVE 	0x12

#ifdef USE_UDP
typedef struct {
  double timestamp;                   // in seconds
  double imu_angular_velocity_rpy[3]; // rad/s -> range: +/- 8192; +/- 2000 deg/se
  double imu_linear_acceleration_xyz[3];    // m/s/s NED, body frame -> sim 1G = 9.80665, FC 1G = 256
  double imu_orientation_quat[4];     //w, x, y, z
  double velocity_xyz[3];             // m/s, earth frame
  double position_xyz[3];             // meters, NED from origin
} fdm_packet;

static udpLink_t stateLink;
#endif

void do_tasks()
{
	bmi160_do_task();
	bmm150_do_task();
}


int main()
{
	int ret;
	extern const struct pios_bmi160_cfg default_bmi160_cfg;

	/* init both sensors */
	const struct pios_bmi160_cfg * cfg = &default_bmi160_cfg;
	if ((ret = PIOS_BMI160_Init(SPIDEV, SPICS, cfg, 0)) < 0) {
		fprintf(stderr, "BMI160 init error %d\n", ret);
	}
	if ((ret = PIOS_BMM150_Init(I2CDEV, I2CSLAVE)) < 0) {
		fprintf(stderr, "BMM150 init error %d\n", ret);
	}

#ifdef USE_UDP
	/* init udp link */
	ret = udpInit(&stateLink, "127.0.0.1", 9003, false);
	printf("start state link...%d\n", ret);

	struct timeval tv;
#endif
	while (1) {
		PIOS_Thread_Sleep(6);		/* XXX */
		/* run task */
		do_tasks();

		/* display all readings */
		//printf("accel: [%f, %f, %f]\n", accel_data.x, accel_data.y, accel_data.z);
		//printf("gyro:  [%f, %f, %f]\n", gyro_data.x, gyro_data.y, gyro_data.z);
		//printf("mag: 	 [%f, %f, %f]\n", mag_data.x, mag_data.y, mag_data.z);

#ifdef USE_UDP
		fdm_packet fdmPkt;
		/* set timestamp */
		if (gettimeofday (&tv, NULL) < 0) {
			perror("gettimeofday failed\n");
		}
		fdmPkt.timestamp = tv.tv_sec;

		/* set imu_angular_velocity_rpy, raw sensor data */
		fdmPkt.imu_angular_velocity_rpy[0] = gyro_data.x;
		fdmPkt.imu_angular_velocity_rpy[1] = gyro_data.y;
		fdmPkt.imu_angular_velocity_rpy[2] = gyro_data.z;

		/* set imu_linear_acceleration_xyz, raw sensor data */
		fdmPkt.imu_linear_acceleration_xyz[0] = accel_data.x;
		fdmPkt.imu_linear_acceleration_xyz[1] = accel_data.y;
		fdmPkt.imu_linear_acceleration_xyz[2] = accel_data.z;

		/* set imu_linear_acceleration_xyz, raw sensor data */
		fdmPkt.imu_orientation_quat[1] = mag_data.x;
		fdmPkt.imu_orientation_quat[2] = mag_data.y;
		fdmPkt.imu_orientation_quat[3] = mag_data.z;

		/* send udp datagram */
		udpSend(&stateLink, &fdmPkt, sizeof(fdmPkt));
#endif
		//PIOS_Thread_Sleep(240);
	}
}
