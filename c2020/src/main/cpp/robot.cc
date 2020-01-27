#include "robot.h"

#include <units/units.h>

namespace team114 {
namespace c2020 {

Robot::Robot()
    : frc::TimedRobot{Robot::kPeriod},
      drive_{Drive::GetInstance()},
      robot_state_{RobotState::GetInstance()} {}

void Robot::RobotInit() {}
void Robot::RobotPeriodic() { drive_.Periodic(); }

void Robot::AutonomousInit() {}
void Robot::AutonomousPeriodic() { drive_.Periodic(); }

void Robot::TeleopInit() {}
void Robot::TeleopPeriodic() { drive_.Periodic(); }

void Robot::TestInit() {}
void Robot::TestPeriodic() {}

void Robot::DisabledInit() {}
void Robot::DisabledPeriodic() {}

}  // namespace c2020
}  // namespace team114

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<team114::c2020::Robot>(); }
#endif
