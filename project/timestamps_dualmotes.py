import matplotlib.pyplot as plt
import statistics
import pandas

sender = 'de23'
receiver = '9dc8'

putty_file_bc = "putty_broadcast1.txt"
putty_file_uc = "putty_unicast1.txt"
putty_files = [putty_file_bc, putty_file_uc]
# putty_file = open("putty_broadcast2.txt", "r")

latencies_rules = []  # list of latencies for both of the observations
means = []
stds = []
for i in range(len(putty_files)):
    file = open(putty_files[i], "r")
    lines = file.readlines()
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
            if len(received) > 2 and int(sent[-2][1]) == int(received[-2][1]) - 1 and int(sent[-2][5]) in \
                    range(int(received[-2][5]) - threshold, int(received[-2][5]) + threshold):
                seqNo = int(sent[-2][1])
                # sender_app_time = int(sender_previous[6])
                sender_app_time = int(sent[-2][6])
                sender_mac_time = int(sent[-1][7])
                sender_monitor_sink = int(sent[-2][8])
                receiver_app_time = int(received[-2][6])
                receiver_mac_time = int(received[-1][7])
                receiver_monitor_sink = int(received[-2][8])

                sender_monitor_latency = sender_mac_time - sender_app_time
                receiver_monitor_latency = receiver_mac_time - receiver_app_time
                measured_latency = receiver_monitor_sink - sender_monitor_sink
                observed_latency = measured_latency + (sender_monitor_latency - receiver_monitor_latency)
                observed_latency_s = observed_latency / (2 ** 15)

                latency = [seqNo, sender_monitor_sink, sender_monitor_latency, receiver_monitor_latency, measured_latency,
                           observed_latency, observed_latency_s]
                latencies.append(latency)

    print(latencies)

    # printf("%x%x,%"PRIu16",%"PRIu16",%"PRIu16",%"PRIu32",%"PRIu16",%"PRIu32",%"PRIu32",%"PRIu32",%lu\n\r",
    # src->u8[sizeof(src->u8) - 2], src->u8[sizeof(src->u8) - 1], received, msg.observed_seqno, msg.monitor_seqno,
    # msg.energy, msg.counter_ADC, msg.timestamp_app, msg.timestamp_mac, timestamp_mac, timestamp_app);
    # de23,13,1289,703,27720,204,138385547,138333350,727530,727529
    # 9dc8,14,9,563,30432,159,138672446,138631768,735679,735678

    time_start = latencies[0][1]
    observed_latencies = [y[-1] * 1000 for y in latencies]  # Observed latencies in ms

    print("\n", putty_files[i])
    plt.xlabel("Elapsed Time [s]")
    # plt.ylabel("Observed latency [ticks]")
    plt.ylabel("Observed latency [ms]")
    plt.title("Latency")
    # plt.plot([(x[1] - time_start) / (1000 * (2**15)) for x in latencies], [y[-1] for y in latencies])
    plt.plot([(x[1] - time_start) / (2 ** 15) for x in latencies], observed_latencies, color='b', linewidth=1, marker='x', markeredgecolor='r', markerfacecolor='r', markersize=6)
    plt.show()

    lat_mean = statistics.mean(observed_latencies)
    means.append(lat_mean)
    lat_var = statistics.variance(observed_latencies)
    lat_std = statistics.stdev(observed_latencies)
    stds.append(lat_std)
    print("\nMean: ", lat_mean, "\nVariance: ", lat_var, "\nStandard deviation: ", lat_std)

    df = pandas.DataFrame(observed_latencies, columns=['Column_Name'])
    print(df.describe())  # statistics about the dataframe

    latencies_rules.append(observed_latencies)
    file.close()

plt.xlabel("LL mode")
plt.ylabel("Observed latency [ms]")
plt.title("Latency")
#.bar(x_pos, CTEs, yerr=error, align='center', alpha=0.5, ecolor='black', capsize=10)
plt.bar(['broadcast', 'unicast'], means, yerr=stds, color='blue', alpha=0.5, capsize=20)
plt.show()
