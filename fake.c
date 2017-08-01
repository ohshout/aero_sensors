#include "udplink.h"
#include <stdio.h>
#include "physical_constants.h"

typedef struct {
  double timestamp;                   // in seconds
  double imu_angular_velocity_rpy[3]; // rad/s -> range: +/- 8192; +/- 2000 deg/se
  double imu_linear_acceleration_xyz[3];    // m/s/s NED, body frame -> sim 1G = 9.80665, FC 1G = 256
  double imu_orientation_quat[4];     //w, x, y, z
  double velocity_xyz[3];             // m/s, earth frame
  double position_xyz[3];             // meters, NED from origin
} fdm_packet;

static udpLink_t stateLink;

int main()
{
	int ret, count = 100;
	struct timeval tv;
	fdm_packet fdmPkt;

	ret = udpInit(&stateLink, "127.0.0.1", 9003, false);
	printf("start state link...%d\n", ret);

	while (1) {
		if (gettimeofday (&tv, NULL) < 0) {
			perror("gettimeofday failed\n");
		}
		fdmPkt.timestamp = tv.tv_sec;
		//count += 100;
		//if (count > 20000) count = 0;
		//fdmPkt.imu_linear_acceleration_xyz[0] = 100 * GRAVITY / 4096.f;
		fdmPkt.imu_linear_acceleration_xyz[0] = 12.80665;
		fdmPkt.imu_linear_acceleration_xyz[1] = 200;
		fdmPkt.imu_linear_acceleration_xyz[2] = 400;

		/* send udp datagram */
		ret=udpSend(&stateLink, &fdmPkt, sizeof(fdmPkt));
		printf("ret=%d\n",ret);
		PIOS_Thread_Sleep(10);
	}
}
