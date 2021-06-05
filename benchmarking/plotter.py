import matplotlib.pyplot as plt
import numpy as np

FILENAMES_FLOPCOUNT = {
    "algo" : "finalBenchmarks/flop-count_scene31_algorithmic.csv",
    "math" : "finalBenchmarks/flop-count_scene31_lto_mathematical.csv", # Andre said I should take this
    "lto" : "finalBenchmarks/flop-count_scene31_lto_mathematical.csv",
    "vect" : "finalBenchmarks/flop-count_scene31_vectorized.csv"
}
FILENAMES_RUNTIME = {
    "base" : "finalBenchmarks/runtimes_scene31_stable_baseline.csv",
    "algo" : "finalBenchmarks/runtimes_scene31_algorithmic.csv",
    "math" : "finalBenchmarks/runtimes_scene31_mathematical.csv",
    "lto" : "finalBenchmarks/runtimes_scene31_lto.csv",
    "vect" : "finalBenchmarks/runtimes_scene31_vectorized.csv",
    "omp" : "finalBenchmarks/runtimes_scene31_vectorized_omp.csv"
}

NR_BYTES_VECTORIZED = 30.5 * 2000000 # bytesperpixel * nr_pixels
#TODO: change this number to correct value
NR_BYTES_NON_VECTORIZED = 30.5 * 2000000
PEAK_PERF_VECTORIZED = 16
PEAK_PERF_NON_VECTORIZED = 4
#TODO: check this value
BYTES_PER_CYCLES = 24

RUNTIME_MAPPING = [
    {
        "name": "Baseline",
        "runtime" : "base",
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "orange"
    },
    {
        "name": "Algorithmic",
        "runtime" : "algo",
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "red"
    },
    {
        "name": "Mathematical",
        "runtime" : "math",
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "blue"
    },
    {
        "name": "Link-time-optimization",
        "runtime" : "lto",
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "yellow"
    },
    {
        "name": "Vectorized",
        "runtime" : "vect",
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "green"
    },
    {
        "name": "Parallelized",
        "runtime" : "omp",
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "brown"
    },
]

PERF_MAPPING = [
    {
        "name": "Algorithmic",
        "runtime" : "algo",
        "flopcount" : "algo",
        "x_coord" : 1700000,
        "y_coord": 1.1,
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "red"
    },
    {
        "name": "Mathematical",
        "runtime" : "math",
        "flopcount" : "math",
        "x_coord" : 1700000,
        "y_coord": 1.4,
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "blue"
    },
    {
        "name": "Link-time-optimization",
        "runtime" : "lto",
        "flopcount" : "lto",
        "x_coord" : 1700000,
        "y_coord": 4,
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "yellow"
    },
    {
        "name": "Vectorized",
        "runtime" : "vect",
        "flopcount" : "vect",
        "x_coord" : 1700000,
        "y_coord": 6.5,
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "green"
    },
    {
        "name": "Parallelized",
        "runtime" : "omp",
        "flopcount" : "vect",
        "x_coord" : 1700000,
        "y_coord": 27,
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "brown"
    },
]

ROOFLINE_MAPPING = [
    {
        "name": "Algorithmic",
        "runtime" : "algo",
        "flopcount" : "algo",
        "nr_bytes" : NR_BYTES_NON_VECTORIZED,
        "peak_perf" : PEAK_PERF_NON_VECTORIZED,
        "b_per_c" : BYTES_PER_CYCLES,
        "point_des" : "o",
        "color" : "red"
    },
    {
        "name": "Mathematical",
        "runtime" : "math",
        "flopcount" : "math",
        "nr_bytes" : NR_BYTES_NON_VECTORIZED,
        "peak_perf" : PEAK_PERF_NON_VECTORIZED,
        "b_per_c" : BYTES_PER_CYCLES,
        "point_des" : "o",
        "color" : "blue"
    },
    {
        "name": "Link-time-optimization",
        "runtime" : "lto",
        "flopcount" : "lto",
        "nr_bytes" : NR_BYTES_NON_VECTORIZED,
        "peak_perf" : PEAK_PERF_NON_VECTORIZED,
        "b_per_c" : BYTES_PER_CYCLES,
        "point_des" : "o",
        "color" : "green"
    },
    {
        "name": "Vectorized",
        "runtime" : "vect",
        "flopcount" : "vect",
        "nr_bytes" : NR_BYTES_NON_VECTORIZED,
        "peak_perf" : PEAK_PERF_NON_VECTORIZED,
        "b_per_c" : BYTES_PER_CYCLES,
        "point_des" : "o",
        "color" : "yellow"
    },
    {
        "name": "Parallelized",
        "runtime" : "omp",
        "flopcount" : "vect",
        "nr_bytes" : NR_BYTES_NON_VECTORIZED,
        "peak_perf" : PEAK_PERF_NON_VECTORIZED,
        "b_per_c" : BYTES_PER_CYCLES,
        "point_des" : "o",
        "color" : "brown"
    },
]


def calc_roofline(flops, bandwidth, n=10, ridge_index=-1):
    x_axis = []
    y_axis = []
    ridge = flops/bandwidth
    if ridge_index == -1:
        ridge_index = n/3
    i = 0
    while(i<ridge_index):
        x_axis.append(ridge*(2**(i - ridge_index)))
        y_axis.append(flops*(2**(i - ridge_index)))
        i+=1
    x_axis.append(ridge)
    y_axis.append(flops)
    i+=1
    while(i < n):
        x_axis.append(ridge*(2**(i - ridge_index + 1)))
        y_axis.append(flops)
        i+=1
    return x_axis, y_axis


def get_flop_counts():
    flop_counts = {}
    for key in FILENAMES_FLOPCOUNT:
        with open(FILENAMES_FLOPCOUNT[key]) as flop_file:
            this_flops = []
            lines = flop_file.readlines()
            for line in lines:
                this_flops.append(float(line.split(",")[-1]))
            flop_counts[key] = this_flops
    return flop_counts


def get_runtimes():
    runtimes = {}
    for key in FILENAMES_RUNTIME:
        with open(FILENAMES_RUNTIME[key]) as runtime_file:
            this_runtimes = []
            lines = runtime_file.readlines()
            for line in lines:
                this_times = []
                times = line.split(",")[2:]
                for time in times:
                    this_times.append(float(time))
                this_runtimes.append(this_times)
            runtimes[key] = this_runtimes
    return runtimes


def get_input_sizes():
    input_sizes = []
    with open(FILENAMES_FLOPCOUNT[list(FILENAMES_FLOPCOUNT.keys())[0]]) as some_file:
        lines = some_file.readlines()
        for line in lines:
            numbers = line.split(",")
            input_sizes.append(float(numbers[0]) * float(numbers[1]))
    return input_sizes


def runtime():
    runtimes = get_runtimes()
    input_sizes = get_input_sizes()

    plt.style.use('seaborn')

    for mapping in RUNTIME_MAPPING:
        runtime = runtimes[mapping['runtime']]
        median = []
        std = []
        for i in range(len(runtime)):
            this_data = []
            for elem in runtime[i]:
                this_data.append(elem)
            median.append(np.median(this_data))
            std.append(np.std(this_data))
        mapping['median'] = median
        mapping['std'] = std
        plt.errorbar(x=input_sizes, y=mapping['median'], 
                yerr=mapping['std'], fmt=mapping['fmt'], 
                color=mapping['color'], capsize=100, label=mapping['name'])
        
    plt.legend(loc="upper right")
    plt.xlabel("Input size (#pixels)")
    plt.ylabel("Runtime [ms]")
    plt.grid(axis="x")
    plt.title("Runtime of different versions with different input sizes \nIntel Core i7-10750H @ 2.6GHz, Memory @ 45.8 GB/s\nSIMD-width: 256 bits",
            {'verticalalignment': 'baseline', 'horizontalalignment': 'left'},
            loc='left', pad=30, fontsize = 15, fontweight='bold'
        )
    #plt.savefig('runtime.eps', format='eps')
    plt.show()


def roofline():
    flop_counts = get_flop_counts()
    runtimes = get_runtimes()

    x_notvector, y_notvector = calc_roofline(4, 25, ridge_index=3)
    x_vector, y_vector = calc_roofline(16, 25, ridge_index=5)
    plt.style.use('seaborn')
    plt.plot(x_vector, y_vector, label="Vector roofline")
    plt.plot(x_notvector, y_notvector, label="Scalar roofline")

    for mapping in ROOFLINE_MAPPING:
        runtime = runtimes[mapping['runtime']][-1]
        flopcount = flop_counts[mapping['flopcount']][-1]
        this_data = []
        for elem in runtime:
            this_data.append((flopcount) / (elem *2.6 *10e6))
        perf = np.median(this_data)
        intensity = flopcount / mapping['nr_bytes']
        print(intensity, perf)
        plt.plot(intensity, perf,
                mapping['point_des'], color=mapping['color'], label=mapping['name'])


    plt.legend(loc="upper right")
    plt.xlabel("Operational Intensity [F/B]")
    plt.xscale("log", base=2)
    plt.ylabel("Performance[F/C]", rotation=0, loc="top", labelpad=-107)
    plt.yscale("log", base=2)
    plt.xticks(ticks=[1/25, 4/25, 16/25,64/25, 256/25], labels=[r'$\dfrac{1}{25}$',r'$\dfrac{4}{25}$',r'$\dfrac{16}{25}$',r'$\dfrac{64}{25}$',r'$\dfrac{256}{25}$'])
    plt.title("Machine @ 2.0GHz with bandwidth of 50 GB/s\nSIMD vector length of 256 bits\nTwo instruction ports, latency 1 cycle", loc="left", pad=20)
    plt.ylim([y_vector[0]/(1.5), 32])
    #plt.savefig('roofline.eps', format='eps')
    plt.show()


def perf_vs_input():
    flop_counts = get_flop_counts()
    runtimes = get_runtimes()
    input_sizes = get_input_sizes()


    plt.style.use('seaborn')
    fig, ax = plt.subplots(1,1)
    fig.set_size_inches(9,7)
    ax.set_yscale('log', basey=2)
    plt.xlim([0, input_sizes[-1] + input_sizes[0]])
    #plt.xticks(
    #    ticks=input_sizes,
    #    labels=["20000", 80000, 180000, 320000, 500000, 720000, 980000, 1280000, 1620000, 2000000]
    #)
    plt.ylim([1, 32])
    plt.yticks(
        ticks=[1, 1.5, 2, 3, 4, 6, 8, 12, 16, 24, 32],
        labels=[r'$1$', r'$1.5$', r'$2$', r'$3$', r'$4$', 
            r'$6$', r'$8$', r'$12$', r'$16$', r'$24$', r'$32$']
    )

    for mapping in PERF_MAPPING:
        runtime = runtimes[mapping['runtime']]
        flopcount = flop_counts[mapping['flopcount']]
        median = []
        std = []
        for i in range(len(runtime)):
            this_data = []
            for elem in runtime[i]:
                this_data.append((flopcount[i] / 1e9) / (elem / 1e3))
            median.append(np.median(this_data))
            std.append(np.std(this_data))
        mapping['median'] = median
        mapping['std'] = std
        plt.errorbar(x=input_sizes, y=mapping['median'], 
                yerr=mapping['std'], fmt=mapping['fmt'], 
                color=mapping['color'], capsize=100,)
        plt.text(x=mapping['x_coord'], y=mapping['y_coord'], s=mapping['name'], fontsize=12)

    plt.xlabel("Input size (#pixels)")
    plt.ylabel("Performance [Gflops/s]")
    plt.grid(axis="x")
    plt.title("Performance of different versions with different input sizes \nIntel Core i7-10750H @ 2.6GHz, Memory @ 45.8 GB/s\nSIMD-width: 256 bits",
            {'verticalalignment': 'baseline', 'horizontalalignment': 'left'},
            loc='left', pad=10, fontsize = 15, fontweight='bold'
        )
    #plt.savefig('perf_vs_input.eps', format='eps')
    plt.show()


if __name__ == "__main__":
    #runtime()
    #roofline()
    perf_vs_input()