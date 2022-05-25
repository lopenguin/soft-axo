d = importData();

% Plot motor data
ax1 = subplot(4,1,1);
plot(d.MOTOR.time,d.MOTOR.potL_adj, 'b', 'DisplayName', 'True Left');
hold on
plot(d.MOTOR.time,d.MOTOR.potR_adj, 'r', 'DisplayName', 'True Right');

plot(d.MOTOR.time,d.MOTOR.targetPotL, 'b-', 'DisplayName', 'Desired Left');
plot(d.MOTOR.time,d.MOTOR.targetPotR, 'r-', 'DisplayName', 'Desired Right');

% create a xline at heel strike
xline(d.FSR.time(d.FSR.val > 450));

xlabel('Time(s)'); ylabel('Motor Angle');
% Plot Gastrocnemius medialis from the EMG data
ax2 = subplot(4,1,2);
plot(trial.fp.Header,trial.fp.Treadmill_R_vy);
xlabel('Time(s)'); ylabel('Y Ground Reaction Force (N)');
% Plot the hip flexion from the inverse kinematics
ax3 = subplot(4,1,3);
plot(trial.ik.Header,trial.ik.ankle_angle_r);
xlabel('Time(s)'); ylabel('Ankle angle (deg)');

% create a xline at heel strike
xline(trial.gcRight.Header(trial.gcRight.HeelStrike==0));

% Plot the hip flexion moment from the inverse dynamics
ax4 = subplot(4,1,4);
plot(trial.id.Header,-trial.id.ankle_angle_r_moment);
xlabel('Time(s)'); ylabel('Ankle moment (N/m)');
linkaxes([ax1 ax2 ax3 ax4],'x')

% create a xline at heel strike
xline(trial.gcRight.Header(trial.gcRight.HeelStrike==0));