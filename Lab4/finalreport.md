# Mobile Communications
## Faculdade de Engenharia da Universidade do Porto
### Network Simulation – WiFi
### Martinho Figueiredo (201506179)
### Pedro Cruz (201504646)
## First Study – Throughput vs distance
For this study we have created the following python code to run and log the results of the scenario described in the first study. The main goal was to visualize the effects of the distance on the throughput of a connection. We concluded the TCP flow decreases with distance until it reaches the 1500 distance, where the throughput showed is zero.

```python
def first_study(start=100,stop=1500,step=100,duration=60, manual=False):
        print('--- First Study ---\n')
    if(manual): 
        print("Input Test Values")
        start = input('Limite Min?')
        stop = input('Limite Max?')
        step = input('Passo?')
        duration = input('Duracao?')
    regex = r"       Throughput: (-\d||\d+.\d+||-\d+.\d+) Kbps"
    rx = re.compile(regex)
    df = pd.DataFrame(columns=['Flow1','Flow2','Distance'])
    for distance in range(start,stop+step,step):
        i = int (distance - start)/step 
        print(f'Distance is {distance}m, iteration {i}, running command:\n    ./waf --run "scratch/first_study --distance={distance} --duration={duration}"')
        process = subprocess.run([f'./waf --run "scratch/first_study --distance={distance} --duration={duration}"'], shell=True, check=True, stdout=subprocess.PIPE, universal_newlines=True)
        output = rx.findall(process.stdout)
        print(i)
        print(process.stdout)
        print(output)
        while(len(output)<2):
            output.append(0)
        print(f'Throughput Results:\n    Flow 1 -> {output[0]} Kbps\n    Flow 2 -> {output[1]} kbps\n\n')
        df.loc[int(i)] = output+[distance]
    df[:] = df[:].astype(float)
    df.plot(x='Distance', kind='line', y=["Flow1","Flow2"], title='Throughput vs Distance')
    return(df)
```
```python
first_study()
```
![results_first_study](img/first_study.)
## Second Study - Throughput vs number of STAs 
In the same way we have created a similar python code to execute and visualise the results. The objective this time is to understand the impact of multiple stations (STAs) in the throughput. In this study we have concluded