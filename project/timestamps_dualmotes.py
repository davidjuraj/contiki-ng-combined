import matplotlib.pyplot as plt
import statistics
import pandas

sender = 'de23'
receiver = '9dc8'
#putty_file = open("putty_broadcast1.txt", "r")
putty_file = open("putty_unicast1.txt", "r")
#putty_file = open("putty_broadcast2.txt", "r")
lines = putty_file.readlines()
lines = filter(lambda x: x.strip(), lines)  # deleting empty lines

# threshold = 30
threshold = 60  # counter_ADC threshold
sent = []  # list of all sent messages
received = []  # list of all received messages
latencies = []
for line in lines:
    if line.startswith(sender):
        line = line.rstrip()
        line = line.split(',')
        sent.append(line)
    elif line.startswith(receiver):
        line = line.rstrip()
        line = line.split(',')
        received.append(line)
        if int(sent[-1][1]) == int(received[-1][1]) - 1 and int(sent[-1][5]) in range(int(received[-1][5]) - threshold,
                                                                                      int(received[-1][
                                                                                              5]) + threshold) and len(
            received) > 1:
            seqNo = int(sent[-1][1])
            # sender_app_time = int(sender_previous[6])
            sender_app_time = int(sent[-2][6])
            sender_mac_time = int(sent[-1][7])
            sender_monitor_sink = int(sent[-1][8])
            receiver_app_time = int(received[-2][6])
            receiver_mac_time = int(received[-1][7])
            receiver_monitor_sink = int(received[-1][8])

            sender_monitor_latency = sender_mac_time - sender_app_time
            receiver_monitor_latency = receiver_mac_time - receiver_app_time
            measured_latency = receiver_monitor_sink - sender_monitor_sink
            observed_latency = measured_latency + (sender_monitor_latency - receiver_monitor_latency)
            observed_latency_s = observed_latency / (2 ** 15)

            latency = [seqNo, sender_monitor_sink, sender_monitor_latency, receiver_monitor_latency, measured_latency,
                       observed_latency, observed_latency_s]
            latencies.append(latency)

print(latencies)

time_start = latencies[0][1]
observed_latencies = [y[-1] * 1000 for y in latencies]  # Observed latencies in ms

plt.xlabel("Elapsed Time [s]")
# plt.ylabel("Observed latency [ticks]")
plt.ylabel("Observed latency [ms]")
plt.title("Latency")
# plt.plot([(x[1] - time_start) / (1000 * (2**15)) for x in latencies], [y[-1] for y in latencies])
plt.plot([(x[1] - time_start) / (2 ** 15) for x in latencies], observed_latencies)
plt.show()

# printf("%x%x,%"PRIu16",%"PRIu16",%"PRIu16",%"PRIu32",%"PRIu16",%"PRIu32",%"PRIu32",%"PRIu32",%lu\n\r",
# src->u8[sizeof(src->u8) - 2], src->u8[sizeof(src->u8) - 1], received, msg.observed_seqno, msg.monitor_seqno,
# msg.energy, msg.counter_ADC, msg.timestamp_app, msg.timestamp_mac, timestamp_mac, timestamp_app);
# de23,13,1289,703,27720,204,138385547,138333350,727530,727529
# 9dc8,14,9,563,30432,159,138672446,138631768,735679,735678

putty_file.close()

lat_mean = statistics.mean(observed_latencies)
lat_var = statistics.variance(observed_latencies)
lat_std = statistics.stdev(observed_latencies)
print("\nMean: ", lat_mean, "\nVariance: ", lat_var, "\nStandard deviation: ", lat_std)

df = pandas.DataFrame(observed_latencies, columns=['Column_Name'])
print(df.describe())
