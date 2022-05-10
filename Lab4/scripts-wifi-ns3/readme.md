# Lab 4
The ns-3 scenarios for the four simulation studies were already created (`first_study.cc`, `second_study.cc`, `third_study.cc` and `fourth_study.cc`) and are located in 

```~/ns-allinone-3.35/ns-3.35/scratch```

##  Usage

Open Terminal and navigate to `ns-3.35` folder

```cd ns-allinone-3.35/ns-3.35```

From there you can run the four studies

For each study, a `.pcap` file of the sink node (e.g., to open in wireshark) is created in the same folder with unique names representing the study number and the values of each relevant configurable parameter.

At the end of the execution of each study, the flow statistics are printed in the console.


## __FIRST STUDY__

Notes:

- You may start at 100m. Lower `distances` result in about the same effective TCP throughput. 
- The maximum link `distance` is expected to be around 1450m. At 1500m there is no connectivity.
- You may collect results with steps of 100m between 100m and 1500m. Decrease the step size if more resolution is needed for the resulting plot($s$).

The default settings are:
> ```duration=10``` ($s$)
>
> ```distance=100``` ($m$)

You may change the parameters directly in the code and run the scenario without any argument:

```./waf --run "scratch/first_study"```

or feed the `distance` ($m$) between nodes and the `duraton` of the flow ($s$) you want to simulate via command line arguments:

```./waf --run "scratch/first_study --distance=100 --duration=60"```

## __SECOND STUDY__

- The coded scenario starts each flow to the sink node between t=2.0s and t=4.0s of simulation time, to avoid collisions.
- Try to simulate using longer `durations` so that the TCP throughput of each flow has a chance of stabilizing.
- You may simulate, for example, up to 15 nodes. When increasing the number of senders, some TCP connection establishments may timeout, leaving some senders idle throughout the simulation.

The default settings are:
> ```duration=10``` ($s$)
>
> ```nr_of_senders=2``` ($nodes$)

You may change the parameters directly in the code and run the scenario without any argument:

```./waf --run "scratch/second_study"```

or feed the `duration` of each flow ($s$) and the `number` of sender nodes you want to simulate via command line arguments:

```./waf --run "scratch/second_study --duration=60  --nr_of_senders=10"```

## __THIRD STUDY__
- Start the UDP flow at 1Mbit/s and increase it by 1Mbit/s steps until the link capacity is clearly exceeded (i.e., the UDP throughput at the sink node will stabilize, independently of how much more traffic is offered at the sender because it is lost).
- When analysing the results, the node with ip=10.0.0.3 is the node sending the UDP flow.

The default settings are:
> `duration=10` ($s$)
>
> `udp_data_rate=1Mbps` 

You may change the parameters directly in the code and run the scenario without any argument:

```./waf --run "scratch/third_study"```

or feed the duration of the TCP and UDP flows ($s$) and the UDP data rate via command line arguments:

```./waf --run "scratch/third_study --duration=60  --udp_data_rate=1Mbps"```

## __FOURTH STUDY__

- You may start at 100m. Lower distances result in the same effective TCP throughput.
- The maximum `distance` between the sender and the sink is expected to be around 2900m. At 3000m there is no connectivity.
- You may collect results with steps of 100m between 100m and 3200m. Decrease the step size if more resolution is needed for the resulting plot($s$).
- Compare the results with the ones obtained in the first study. Place them side by side in the same plot and elaborate some conclusions.

The default settings are:
> `duration=10` ($s$)
>
> `distance=100` ($m$)

You may change the parameters directly in the code and run the scenario without any argument:

`./waf --run "scratch/fourth_study`

or feed the `distance` ($m$) between sender and sink nodes (relay is placed at `distance/2`) and the duraton of the flow ($s$) you want to simulate via command line arguments:

`./waf --run "scratch/fourth_study --distance=100 --duration=60"`



