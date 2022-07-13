clc, close all, clear all

%d = importdata("putty_log.txt");
%system("python test_filter.py > outfrompy.txt")
%d_from_py = importdata("outfrompy.txt");
d = importdata("raw_data.txt");
good_samples_start = 69000;
good_samples_stop = 71000;
data = d(good_samples_start:good_samples_stop);
data = d;
fs = 1000;
t = linspace(0, length(data)/fs, length(data));

[b,a] = butter(1, [0.5, 3] / (fs / 2));
[~,state] = filter(b,a,ones(1,1000) * data(1));
filtered_data = filter(b,a,data, state);

plot(t,filtered_data)
figure
plot(t, data)
figure

bucket_size = 1000;
runningavgb = ones(1, bucket_size)/bucket_size;
[~, ravg_state] = filter(runningavgb,1,ones(1,bucket_size)*filtered_data(1)*filtered_data(2));
running_avg = filter(runningavgb,1,filtered_data(1:end-5).*filtered_data(6:end), ravg_state);
%Omega = atan(((bucket_size-1))./sum(d(1:end-1,:).*d(2:end,:)));
%omega = Omega*fs;
%f = omega/2/pi;
%bpm = 60*f;
plot(t(1:end-5), atan(1./running_avg)*fs/2/pi*60, t, filtered_data)
%% find peaks
close all, clc

figure 
[~,ip] = findpeaks(filtered_data, "MinPeakProminence",10);
hold on 
plot(t, filtered_data)
scatter(t(ip(2:end)), fs./diff(ip)*60)

% diy approach
highest_peak = filtered_data(1);
highest_peak_i = 1;
lowest_valley_after = 0;
lowest_valley_before = inf;

has_peak = false;
vals = [filtered_data(3) filtered_data(2) filtered_data(1)];
peaks = [];
min_val = filtered_data(1);
max_val = filtered_data(1);
pp_change = 0.5^(1/1000);
tmp = zeros(3, length(filtered_data));

for i = 4:length(filtered_data)
    vals = [filtered_data(i) vals(1:2)];
    min_val = min(min_val*pp_change, vals(1));
    max_val = max(max_val*pp_change, vals(1));
    peak_to_peak = max_val - min_val;
    min_prom = peak_to_peak * 0.5;
    tmp(:,i) = [min_val max_val min_prom];

    if vals(1) < lowest_valley_after
        lowest_valley_after = vals(1);
    end
    if ~has_peak && vals(1) < lowest_valley_before
        lowest_valley_before = vals(1);
    end
    if vals(2) > vals(1) && vals(2) > vals(3)
        if highest_peak < vals(2) && lowest_valley_before + min_prom < vals(2) 
            highest_peak = vals(2);
            highest_peak_i = i-1;
            has_peak = true;
            lowest_valley_before = min(lowest_valley_before, lowest_valley_after);
        end
        lowest_valley_after = inf;
    elseif has_peak
        if lowest_valley_before + min_prom < highest_peak
            if lowest_valley_after + min_prom < highest_peak
                peaks(end+1) = highest_peak_i;
                highest_peak = -inf;
                lowest_valley_before = inf;
                has_peak = false;
            end
        end
    end
end

figure
scatter(t(peaks(2:end)), fs./diff(peaks)*60)
hold on 
plot(t,filtered_data)
%scatter(peaks, filtered_data(peaks))
plot(t, tmp')

%%
% lets try a lsqfit nope
f = @(a, x) a(1).*sin(a(2).*x + a(3));

amp = (max(filtered_data) - min(filtered_data))/2;
a0 = [amp 2*pi 0];
fit = lsqcurvefit(f, a0, t', filtered_data);
figure
plot(t, filtered_data, t, f(fit, t))

%%
% lets try pll nope
figure
[lb, la] = butter(2, .05/(fs/2));
nco_f = 1;
[~, lf_state] = filter(lb, la, 0);
ncofs = [];
es = [];
ctrls = [];
ncos = [];
for i = 1:length(t)
    nco = sin(2*pi*t(i)*nco_f);
    e = atan(filtered_data(i) / nco);
    [ctrl, lf_state] = filter(lb, la, e, lf_state);
    nco_f = nco_f + ctrl/1000;
    ncos(end+1) = nco;
    es(end+1) = e;
    ncofs(end+1) = nco_f; 
    ctrls(end+1) = ctrl;
end
plot(t, ncofs, t,es)

%% fft
figure
freq = abs(fft(filtered_data));
freqHz = (0:1:length(freq)/2-1)*fs/length(filtered_data);
plot(freqHz*60, freq(1:end/2))

