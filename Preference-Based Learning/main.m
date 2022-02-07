%% Script: main
% Description: Runs main PBL learning algorithm
%
% Author: Maegan Tucker (mtucker@caltech.edu)
% ________________________________________

%% Add POLAR toolbox to path

polar_path = fullfile('../../POLAR'); % modify this to the location of your POLAR repository
addpath(polar_path); % add POLAR to your path
toolbox_addpath % add toolbox path

%% Define user settings

%%%%%% general settings
settings.save_folder = 'saved_results'; % specify folder where results will be saved

settings.b = 1; % number of past actions to compare with current trial
settings.n = 1; % number of actions to sample in each trial
settings.acq_type = 1; % 1 for regret minimization, 2 for active learning, 3 for random sampling
settings.feedback_type = 1; %include 1 for preferences, 2 for suggestions, and 3 for ordinal labels
settings.useSubset = 0; % 0 for no dim reduction, 1 for dim reduction

%%%%%% simulation settings
settings.maxIter = 10; % max number of iterations for simulations
settings.simulated_pref_noise = 0.02; % synthetic feedback noise parameter
settings.simulated_coac_noise = 0.04; % synthetic feedback noise parameter


%%%%%%  Parameter settings - add as many as you need

% Dimension 1 
ind = 1;
settings.parameters(ind).name = 'Gait Percent'; 
settings.parameters(ind).discretization = 0.01;
settings.parameters(ind).lower = 0.3;
settings.parameters(ind).upper = 0.5;
settings.parameters(ind).lengthscale = 0.01;

%%%%%%  Hyperparameters
settings.linkfunction = 'sigmoid';
settings.signal_variance = 1;   % Gaussian process amplitude parameter
settings.post_pref_noise = 0.02;    % How noisy are the user's preferences?
settings.post_coac_noise = 0.04;    % How noisy are the user's suggestions?
settings.post_ord_noise = 0.1;      % How noisy are the user's labels?
settings.GP_noise_var = 1e-4;       % GP model noise--need at least a very small

alg = PBL(settings);

%% Run Experiment
% export_folder = 'example_yamls'; %optional input - location for writing yaml files of actions to sample
plottingFlag = 0; %flag for showing plots during experiment 
isSave = 1; % flag for saving results
alg.runExperiment(plottingFlag,isSave);


