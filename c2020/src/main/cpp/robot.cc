#include "robot.h"

#include <units/units.h>

namespace team114 {
namespace c2020 {

Robot::Robot()
    : frc::TimedRobot{Robot::kPeriod},
      drive_{Drive::GetInstance()},
      robot_state_{RobotState::GetInstance()},
      ljoy_{0},
      rjoy_{1},
      auto_selector_{auton::AutoModeSelector::GetInstance()},
      auto_executor_{std::make_unique<auton::EmptyAction>()},
      cfg{conf::GetConfig()},
      intake_rot{cfg.intake.rot_talon_id},
      intake_roller{cfg.intake.roller_talon_id},
      channel_ser{cfg.ball_channel.serializer_id},
      channel_chan{cfg.ball_channel.channel_id},
      shooter_1{cfg.shooter.master_id},
      shooter_2{cfg.shooter.slave_id},
      shooter_kicker{cfg.shooter.kicker_id},
      climber_1{cfg.climber.master_id},
      climber_2{cfg.climber.slave_id},
      climber_brake{6},
      climber_latch{7} {}

void Robot::RobotInit() {}
void Robot::RobotPeriodic() { drive_.Periodic(); }

void Robot::AutonomousInit() {
    auto mode = auto_selector_.GetSelectedAction();  // heh
    auto_executor_ = auton::AutoExecutor{std::move(mode)};
}
void Robot::AutonomousPeriodic() { auto_executor_.Periodic(); }

void Robot::TeleopInit() {}
void Robot::TeleopPeriodic() {
    SDB_NUMERIC(double, JoystickThrottle) throttle = ljoy_.GetRawAxis(1);
    SDB_NUMERIC(double, JoystickWheel) wheel = -rjoy_.GetRawAxis(0);
    SDB_BOOL(JoystickQuickturn) quick_turn = rjoy_.GetRawButton(1);
    drive_.SetWantCheesyDrive(throttle, wheel, quick_turn);
}

void Robot::TestInit() {
    shooter_1.SetNeutralMode(NeutralMode::Coast);
    shooter_2.SetNeutralMode(NeutralMode::Coast);
}
void Robot::TestPeriodic() {
    READING_SDB_NUMERIC(double, IntakeRotCmd) intake_rot_cmd;
    intake_rot.Set(ControlMode::PercentOutput, intake_rot_cmd);
    READING_SDB_NUMERIC(double, IntakeRollCmd) intake_roller_cmd;
    intake_roller.Set(ControlMode::PercentOutput, intake_roller_cmd);
    READING_SDB_NUMERIC(double, ChannelSerCmd) channel_ser_cmd;
    channel_ser.Set(ControlMode::PercentOutput, channel_ser_cmd);
    READING_SDB_NUMERIC(double, ChannelChanCmd) channel_chan_cmd;
    channel_chan.Set(ControlMode::PercentOutput, channel_chan_cmd);
    READING_SDB_NUMERIC(double, ShooterCmd) shooter_cmd;
    shooter_1.Set(ControlMode::PercentOutput, shooter_cmd);
    shooter_2.Set(ControlMode::PercentOutput, shooter_cmd);
    READING_SDB_NUMERIC(double, KickerCmd) kicker_cmd;
    shooter_kicker.Set(ControlMode::PercentOutput, kicker_cmd);

    // CLIMBER
    // READING_SDB_NUMERIC(double, ClimberCmd) climber_cmd;
    // READING_SDB_NUMERIC(double, ClimberBrake) climber_brake_c;
    // READING_SDB_NUMERIC(double, ClimberLatch) climber_latch_c;
    // climber_1.Set(ControlMode::PercentOutput, climber_cmd);
    // climber_2.Set(ControlMode::PercentOutput, climber_cmd);
    // climber_brake.Set(climber_brake_c > 0);
    // climber_latch.Set(climber_latch_c > 0);
}

void Robot::DisabledInit() { auto_executor_.Stop(); }
void Robot::DisabledPeriodic() { auto_selector_.UpdateSelection(); }

}  // namespace c2020
}  // namespace team114

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<team114::c2020::Robot>(); }
#endif
