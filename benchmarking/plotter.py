import matplotlib.pyplot as plt
import numpy as np
from curlyBrace import curlyBrace

FILENAMES_FLOPCOUNT = {
    "base" : "finalBenchmarks/flop-count_scene34_base.csv",
    "algo" : "finalBenchmarks/flop-count_scene34_alg.csv",
    #"math" : "finalBenchmarks/flop-count_scene31_lto_mathematical.csv", # Andre said I should take this
    "lto" : "finalBenchmarks/flop-count_scene34_lto.csv",
    "vect" : "finalBenchmarks/flop-count_scene34_vect.csv",
    #"vect_16" : "benchmarks_scenechange/flop-count_scene34_vect.csv"
}
FILENAMES_RUNTIME = {
    "base" : "finalBenchmarks/runtimes_scene34_base.csv",
    "algo" : "finalBenchmarks/runtimes_scene34_alg.csv",
    #"math" : "finalBenchmarks/runtimes_scene31_mathematical.csv",
    "lto" : "finalBenchmarks/runtimes_scene34_lto.csv",
    "vect" : "finalBenchmarks/runtimes_scene34_vect.csv",
    "omp" : "finalBenchmarks/runtimes_scene34_vect_omp.csv",
    #"vect_16" : "benchmarks_scenechange/runtimes_scene34_vect.csv"
}

FILENAMES_SCENES_RUNTIMES_LTO = {
    "lto_4" : "finalBenchmarks/size_comp/runtimes_scene31_lto.csv",
    "lto_8" : "finalBenchmarks/size_comp/runtimes_scene32_lto.csv",
    #"lto_12" : "finalBenchmarks/size_comp/runtimes_scene33_lto.csv",
    "lto_16" : "finalBenchmarks/size_comp/runtimes_scene34_lto.csv",
    #"lto_20" : "finalBenchmarks/size_comp/runtimes_scene35_lto.csv",
    "lto_32" : "finalBenchmarks/size_comp/runtimes_scene36_lto.csv",
    "lto_64" : "finalBenchmarks/size_comp/runtimes_scene37_lto.csv"
}

FILENAMES_SCENES_RUNTIMES_VECT = {
     
    "vect_4" : "finalBenchmarks/size_comp/runtimes_scene31_vect.csv",
    "vect_8" : "finalBenchmarks/size_comp/runtimes_scene32_vect.csv",
    #"vect_12" : "finalBenchmarks/size_comp/runtimes_scene33_vect.csv",
    "vect_16" : "finalBenchmarks/size_comp/runtimes_scene34_vect.csv",
    #"vect_20" : "finalBenchmarks/size_comp/runtimes_scene35_vect.csv",
    "vect_32" : "finalBenchmarks/size_comp/runtimes_scene36_vect.csv",
    "vect_64" : "finalBenchmarks/size_comp/runtimes_scene37_vect.csv",
}

FILENAMES_SCENES_FLOPS_LTO = {
    "lto_4" : "finalBenchmarks/size_comp/flop-count_scene31_lto.csv",
    "lto_8" : "finalBenchmarks/size_comp/flop-count_scene32_lto.csv",
    #"lto_12" : "finalBenchmarks/size_comp/flop-count_scene33_lto.csv",
    "lto_16" : "finalBenchmarks/size_comp/flop-count_scene34_lto.csv",
    #"lto_20" : "finalBenchmarks/size_comp/flop-count_scene35_lto.csv",
    "lto_32" : "finalBenchmarks/size_comp/flop-count_scene36_lto.csv",
    "lto_64" : "finalBenchmarks/size_comp/flop-count_scene37_lto.csv"
}

FILENAMES_SCENES_FLOPS_VECT = {
    "vect_4" : "finalBenchmarks/size_comp/flop-count_scene31_vect.csv",
    "vect_8" : "finalBenchmarks/size_comp/flop-count_scene32_vect.csv",
    #"vect_12" : "finalBenchmarks/size_comp/flop-count_scene33_vect.csv",
    "vect_16" : "finalBenchmarks/size_comp/flop-count_scene34_vect.csv",
    #"vect_20" : "finalBenchmarks/size_comp/flop-count_scene35_vect.csv",
    "vect_32" : "finalBenchmarks/size_comp/flop-count_scene36_vect.csv",
    "vect_64" : "finalBenchmarks/size_comp/flop-count_scene37_vect.csv",
}


NR_BYTES_VECTORIZED = 30.5 * 2000000 # bytesperpixel * nr_pixels
#TODO: change this number to correct value
NR_BYTES_NON_VECTORIZED = 30.5 * 2000000
PEAK_PERF_VECTORIZED = 16
PEAK_PERF_NON_VECTORIZED = 4
#TODO: check this value
BYTES_PER_CYCLES = 17.61538

RUNTIME_MAPPING = [
    {
        "name": "Baseline",
        "runtime" : "base",
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "black",
        "marker" : "X"
    },
    {
        "name": "Algorithmic",
        "runtime" : "algo",
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "navy",
        "marker" : "^"
    },
    #{
    #    "name": "Mathematical",
    #    "runtime" : "math",
    #    "median" : [],
    #    "std" : [],
    #    "fmt" : "-D",
    #    "color" : "navy",
    #    "marker" : "^"
    #},
    {
        "name": "Link-time-optimized",
        "runtime" : "lto",
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "olive",
        "marker" : "P"
    },
    {
        "name": "Vectorized",
        "runtime" : "vect",
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "darkgreen",
        "marker" : "o"
    },
    {
        "name": "Parallelized",
        "runtime" : "omp",
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "brown",
        "marker" : "s"
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
        "color" : "navy",
        "marker" : "^"
    },
    #{
    #    "name": "Mathematical",
    #    "runtime" : "math",
    #    "flopcount" : "math",
    #    "x_coord" : 1700000,
    #    "y_coord": 1.3,
    #    "median" : [],
    #    "std" : [],
    #    "fmt" : "-D",
    #    "color" : "navy"
    #},
    {
        "name": "Link-time-optimized",
        "runtime" : "lto",
        "flopcount" : "lto",
        "x_coord" : 1700000,
        "y_coord": 4,
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "olive",
        "marker" : "P"
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
        "color" : "darkgreen",
        "marker" : "o"
    },
    {
        "name": "Parallelized",
        "runtime" : "omp",
        "flopcount" : "vect",
        "x_coord" : 1700000,
        "y_coord": 26,
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "brown",
        "marker" : "s"
    },
]

ROOFLINE_MAPPING = [
    {
        "name": "Baseline",
        "runtime" : "base",
        "flopcount" : "base",
        "nr_bytes" : NR_BYTES_NON_VECTORIZED,
        "peak_perf" : PEAK_PERF_NON_VECTORIZED,
        "b_per_c" : BYTES_PER_CYCLES,
        "point_des" : "X",
        "color" : "black"
    },
    {
        "name": "Algorithmic",
        "runtime" : "algo",
        "flopcount" : "algo",
        "nr_bytes" : NR_BYTES_NON_VECTORIZED,
        "peak_perf" : PEAK_PERF_NON_VECTORIZED,
        "b_per_c" : BYTES_PER_CYCLES,
        "point_des" : "^",
        "color" : "navy"
    },
    #{
    #    "name": "Mathematical",
    #    "runtime" : "math",
    #    "flopcount" : "math",
    #    "nr_bytes" : NR_BYTES_NON_VECTORIZED,
    #    "peak_perf" : PEAK_PERF_NON_VECTORIZED,
    #    "b_per_c" : BYTES_PER_CYCLES,
    #    "point_des" : "X",
    #    "color" : "navy"
    #},
    {
        "name": "Link-time-optimized",
        "runtime" : "lto",
        "flopcount" : "lto",
        "nr_bytes" : NR_BYTES_NON_VECTORIZED,
        "peak_perf" : PEAK_PERF_NON_VECTORIZED,
        "b_per_c" : BYTES_PER_CYCLES,
        "point_des" : "P",
        "color" : "olive"
    },
    {
        "name": "Vectorized",
        "runtime" : "vect",
        "flopcount" : "vect",
        "nr_bytes" : NR_BYTES_NON_VECTORIZED,
        "peak_perf" : PEAK_PERF_NON_VECTORIZED,
        "b_per_c" : BYTES_PER_CYCLES,
        "point_des" : "o",
        "color" : "darkgreen"
    },
    {
        "name": "Parallelized",
        "runtime" : "omp",
        "flopcount" : "vect",
        "nr_bytes" : NR_BYTES_NON_VECTORIZED,
        "peak_perf" : PEAK_PERF_NON_VECTORIZED,
        "b_per_c" : BYTES_PER_CYCLES,
        "point_des" : "s",
        "color" : "brown"
    },
    #{
    #    "name": "Vectorized - Larger scene",
    #    "runtime" : "vect_16",
    #    "flopcount" : "vect_16",
    #    "nr_bytes" : NR_BYTES_NON_VECTORIZED,
    #    "peak_perf" : PEAK_PERF_NON_VECTORIZED,
    #    "b_per_c" : BYTES_PER_CYCLES,
    #    "point_des" : "s",
    #    "color" : "darkgreen"
    #},
]


SCENES_MAPPING = [
    {
        "name" : "Vectorized",
        "peak_perf" : PEAK_PERF_VECTORIZED,
        "color" : "darkgreen",
        "marker" : "o"
    },
    {
        "name" : "Link-time optimized",
        "peak_perf" : PEAK_PERF_NON_VECTORIZED,
        "color" : "olive",
        "marker" : "P"
    }
]

def calc_roofline(flops, bandwidth, n=10, ridge_index=-1, x_dist=1):
    x_axis = []
    y_axis = []
    ridge = flops/bandwidth
    if ridge_index == -1:
        ridge_index = n/10
    i = 0
    while(i<ridge_index):
        x_axis.append(ridge*(2**(i - ridge_index - x_dist)))
        y_axis.append(flops*(2**(i - ridge_index - x_dist)))
        i+=1
    x_axis.append(ridge)
    y_axis.append(flops)
    i+=1
    while(i < n):
        x_axis.append(ridge*(2**(i - ridge_index + x_dist + 1)))
        y_axis.append(flops)
        i+=1
    return x_axis, y_axis


def get_flop_counts(filenames_flopcount = FILENAMES_FLOPCOUNT):
    flop_counts = {}
    for key in filenames_flopcount:
        with open(filenames_flopcount[key]) as flop_file:
            this_flops = []
            lines = flop_file.readlines()
            for line in lines:
                this_flops.append(float(line.split(",")[-1]))
            flop_counts[key] = this_flops
    return flop_counts


def get_runtimes(file_names_runtime = FILENAMES_RUNTIME):
    runtimes = {}
    for key in file_names_runtime:
        with open(file_names_runtime[key]) as runtime_file:
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

def get_image_width():
    widths = []
    with open(FILENAMES_FLOPCOUNT[list(FILENAMES_FLOPCOUNT.keys())[0]]) as some_file:
        lines = some_file.readlines()
        for line in lines:
            numbers = line.split(",")
            widths.append(float(numbers[0]))
    return widths


def runtime():
    runtimes = get_runtimes()
    input_sizes = get_image_width()

    # set the plot properties
    plt.style.use('seaborn')
    fig, ax = plt.subplots(1,1)
    fig.set_size_inches(10,6)
    ax.set_yscale('log', basey=10) # only use logarithmic scale for y

    # limit the axes and set the y-axis ticks
    plt.xlim([150, 2250])
    plt.ylim([0.006, 3000])
    plt.yticks(ticks=[0.01, 0.1, 1, 10, 100, 1000], labels=["0.01", "0.1", "1", "10", "100", "1000"], fontsize=18)
    plt.xticks(ticks=input_sizes, labels=[200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 2000], fontsize=18)

    # title, x,y-axis labels and grid
    plt.xlabel("Image Width [Pixels]", fontsize=20)
    plt.ylabel("Execution Time [s]", fontsize=20, loc='top', rotation=0, labelpad=-210)
    plt.grid(b=None, which='major', axis='y', color='w')
    plt.grid(b=False, axis='x')
    plt.title(
        "Execution Times on Different Image Sizes \nIntel Core i7-10750H @ 2.6 GHz, Memory @ 45.8 GB/s",
        {
            'verticalalignment': 'baseline',
            'horizontalalignment': 'left'
        }, 
        loc = 'left',
        pad = 40,
        fontsize = 20,
        fontweight = 'bold'
    )
    plt.tight_layout()
    perf = [] 
    naming = []

    # plot the actual data
    for mapping in RUNTIME_MAPPING:
        runtime = runtimes[mapping['runtime']]
        median = []
        std = []
        for i in range(len(runtime)):
            this_data = []
            for elem in runtime[i]:
                this_data.append(elem/1000)
            median.append(np.median(this_data))
            std.append(np.std(this_data))
        mapping['median'] = median
        mapping['std'] = std
        perf.append(median[-1])
        naming.append(mapping['name'])

        # plot the data without mapping names
        plt.plot(
            input_sizes, 
            mapping['median'], 
            marker=mapping["marker"],
            color=mapping['color'], 
            #label=mapping['name']
        )

    rot=2

    plt.text(650, 290, "Baseline", fontfamily="sans-serif", fontweight="bold", fontsize="15", rotation=rot)
    plt.text(1000, 120, "Algorithmic", fontfamily="sans-serif", fontweight="bold", fontsize="15", rotation=rot)
    plt.text(650, 12, "Mathematical", fontfamily="sans-serif", fontweight="bold", fontsize="15", rotation=rot*1.5)
    plt.text(1000, 2.9, "Vectorized", fontfamily="sans-serif", fontweight="bold", fontsize="15", rotation=rot)
    plt.text(650, 0.18, "Multi-Threaded", fontfamily="sans-serif", fontweight="bold", fontsize="15", rotation=rot)
    plt.plot([860, 985], [410, 425], linestyle="dotted", color="black")
    plt.plot([1275, 1385], [165, 133], linestyle="dotted", color="black")
    plt.plot([960, 1185], [18, 22], linestyle="dotted", color="black")
    plt.plot([990, 815], [3.7, 3.8], linestyle="dotted", color="black")
    plt.plot([640, 415], [0.22, 0.188], linestyle="dotted", color="black")

    fontdic = {
        'rotation' : 1,
        'color' : 'maroon'
    }

    curlyBrace(fig, ax, [2020, 1530], [2020,4.6], str_text="", color='maroon', fontdict=fontdic)
    plt.text(2110, 66, "327x", color='maroon', fontsize=15)

    print("Runtime in s")
    for i in range(len(perf)):
        if i == 0:
            print(f"Runtime {naming[i]:15.15}: {perf[i]:6.5}")
        else:
            print(f"Runtime {naming[i]:15.15}:  {perf[i]:6.5}\n\
    Speedup to version before: {perf[i-1]/perf[i]:6.5}\n\
    Speedup to baseline:       {perf[0]/perf[i]:6.5}")

    plt.savefig('runtime-plot.eps', format='eps')
    plt.show()


def roofline():
    flop_counts = get_flop_counts()
    runtimes = get_runtimes()

    x_seq, y_seq = calc_roofline(4, 17.61, n=50, ridge_index=2, x_dist=2)
    x_simd, y_simd = calc_roofline(16, 17.61, n=50, ridge_index=4, x_dist=2)
    x_par, y_par = calc_roofline(96, 17.61, n=50, ridge_index=7, x_dist=2)
    
    # set the plot properties
    plt.style.use('seaborn')
    fig, ax = plt.subplots(1,1)
    fig.set_size_inches(10,6)
    ax.set_xscale('log', basex=2)
    ax.set_yscale('log', basey=2)

    # limit the axes and set the axis ticks
    plt.xlim([1/64, 2**16])
    plt.ylim([0.125, 170])
    plt.xticks(
        ticks=[1/64, 1/16, 1/4, 1, 4, 16, 64, 256, 1024, 4096, 16384, 65536],
        labels=[r'$\dfrac{1}{64}$', r'$\dfrac{1}{16}$', r'$\dfrac{1}{4}$', r"$1$", "4", "16", "64", "256", "1,024", "4,096", "16,384", "65,536"],
        fontsize=18
    )
    plt.yticks(
        ticks=[1/4, 1, 4, 16, 64],
        labels=[r'$\dfrac{1}{4}$', "1", "4", "16", "64"],
        fontsize=18
    )

    # title, x,y-axis labels and grid
    plt.xlabel("Operational Intensity [Flops/Byte]", fontsize=20)
    plt.ylabel("Performance [Flops/Cycle]", fontsize=20, loc='top', rotation=0, labelpad=-262)
    plt.grid(b=None, which='major', axis='y', color='w')
    plt.grid(b=False, axis='x')
    #plt.grid(b=None, which='major', axis='y', color='w')
    plt.title(
        "Roofline Plot considering only Compulsory Misses \nIntel Core i7-10750H @ 2.6 GHz, Memory @ 45.8 GB/s",
        {
            'verticalalignment': 'baseline',
            'horizontalalignment': 'left'
        }, 
        loc = 'left',
        pad = 40,
        fontsize = 20,
        fontweight = 'bold'
    )
    plt.tight_layout()
    
    plt.plot(x_simd, y_simd, '--', linewidth=1.5, color='maroon')
    plt.plot(x_seq, y_seq, '-', linewidth=1.5, color='maroon')
    plt.plot(x_par, y_par, ':', linewidth=1.5, color='maroon')

    plt.text(6, 105, "Multi-Core Roofline", fontfamily="sans-serif", color="maroon", fontsize="15", fontstyle="italic")
    plt.text(6, 18, "Vector Roofline", fontfamily="sans-serif", color="maroon", fontsize="15", fontstyle="italic")
    plt.text(6, 4.5, "Scalar Roofline", fontfamily="sans-serif", color="maroon", fontsize="15", fontstyle="italic")

    perf_arr = [] 
    naming = []
    intens_arr = []

    # plot actual data
    for mapping in ROOFLINE_MAPPING:
        runtime = runtimes[mapping['runtime']][-1]
        flopcount = flop_counts[mapping['flopcount']][-1]
        this_data = []
        for elem in runtime:
            this_data.append((flopcount) / (elem * 2.6 * 1e6))
        perf = np.median(this_data)
        intensity = flopcount / mapping['nr_bytes']
        perf_arr.append(perf)
        intens_arr.append(intensity)
        naming.append(mapping['name'])
        plt.plot(intensity, perf,
                mapping['point_des'], color=mapping['color'], label=mapping['name'])

    plt.text(3000, 0.2, "Baseline", fontfamily="sans-serif", fontweight="bold", fontsize="16")
    plt.text(300, 0.55, "Algorithmic", fontfamily="sans-serif", fontweight="bold", fontsize="16")
    plt.text(200, 2, "Mathematical", fontfamily="sans-serif", fontweight="bold", fontsize="16")
    plt.text(400, 7, "Vectorized", fontfamily="sans-serif", fontweight="bold", fontsize="16")
    plt.text(150, 33, "Multi-Threaded", fontfamily="sans-serif", fontweight="bold", fontsize="16")
    plt.plot([14000, 29000], [0.27, 0.46], linestyle="dotted", color="black")
    plt.plot([2650, 5500], [0.63, 0.61], linestyle="dotted", color="black")
    plt.plot([2300, 4800], [2.3, 2.35], linestyle="dotted", color="black")
    plt.plot([2900, 5500], [7.6, 5.8], linestyle="dotted", color="black")
    plt.plot([2400, 5200], [37, 32], linestyle="dotted", color="black")

    print("Performance in Flops/Cycle")
    for i in range(len(perf_arr)):
        if i == 0:
            print(f"Perf {naming[i]:15.15}: {perf_arr[i]:6.5}")
        else:
            print(f"Perf {naming[i]:15.15}:  {perf_arr[i]:6.5}\n\
    Diff to version before: {perf_arr[i]/perf_arr[i-1]:6.5}\n\
    Diff to baseline:       {perf_arr[i]/perf_arr[0]:6.5}")

    print("Operational Intensity in Flops/Byte")
    for i in range(len(perf_arr)):
        if i == 0:
            print(f"Intens {naming[i]:15.15}: {intens_arr[i]:6.6}")
        else:
            print(f"Intens {naming[i]:15.15}:  {intens_arr[i]:6.5}\n\
    Diff to version before: {intens_arr[i]/intens_arr[i-1]:6.5}\n\
    Diff to baseline:       {intens_arr[i]/intens_arr[0]:6.5}")

    plt.savefig('roofline.eps', format='eps')
    plt.show()


def perf_vs_input():
    flop_counts = get_flop_counts()
    runtimes = get_runtimes()
    input_sizes = get_image_width()

    # set the plot properties
    plt.style.use('seaborn')
    fig, ax = plt.subplots(1,1)
    fig.set_size_inches(10,6)
    ax.set_yscale('log', basey=2)

    # limit the axes and set the axis ticks 
    plt.xlim([150, 2050])
    plt.ylim([0.7, 130])# 43])
    plt.yticks(ticks=[1,2,4,8,16,32,64, 128], labels=["1", "2", "4", "8", "16", "32", "64", "128"], fontsize=18)
    plt.xticks(ticks=input_sizes, labels=[200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 2000], fontsize=18)

    # title, x,y-axis labels and grid
    plt.xlabel("Image Width [Pixels]", fontsize=20)
    plt.ylabel("Performance [GFlop/s]", fontsize=20, loc='top', rotation=0, labelpad=-238)
    plt.grid(b=None, which='major', axis='y', color='w')
    plt.grid(b=False, axis='x')
    plt.title(
        "Performance on Different Image Sizes \nIntel Core i7-10750H @ 2.6 GHz, Memory @ 45.8 GB/s",
        {
            'verticalalignment': 'baseline',
            'horizontalalignment': 'left'
        }, 
        loc = 'left',
        pad = 40,
        fontsize = 20,
        fontweight = 'bold'
    )
    plt.tight_layout()  
    perf = [] 
    naming = [] 

    # plot the actual data
    for mapping in PERF_MAPPING:
        print(mapping['name'])
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
        perf.append(median[-1])
        naming.append(mapping['name'])
        mapping['std'] = std
        plt.errorbar(x=input_sizes, y=mapping['median'], 
                yerr=mapping['std'], marker=mapping['marker'], 
                color=mapping['color'], capsize=100,)

    plt.text(1300, 1.9, "Algorithmic", fontfamily="sans-serif", fontweight="bold", fontsize="16")
    plt.text(1300, 7.5, "Mathematical", fontfamily="sans-serif", fontweight="bold", fontsize="16")
    plt.text(1300, 17.5, "Vectorized", fontfamily="sans-serif", fontweight="bold", fontsize="16")
    plt.text(1300, 97, "Multi-Threaded", fontfamily="sans-serif", fontweight="bold", fontsize="16") 
    plt.plot([1210, 1295], [1.7, 2.1], linestyle="dotted", color="black")
    plt.plot([1210, 1295], [6.6, 8.2], linestyle="dotted", color="black")
    plt.plot([1210, 1295], [15.5, 19.4], linestyle="dotted", color="black")
    plt.plot([1210, 1295], [88, 105], linestyle="dotted", color="black")
    
    print("Performance in GFlops/s")
    for i in range(len(perf)):
        if i == 0:
            print(f"Perf {naming[i]:15.15}: {perf[i]:6.5}")
        else:
            print(f"Perf {naming[i]:15.15}:  {perf[i]:6.5}\n\
    Perf inc to version before: {perf[i]/perf[i-1]:6.5}\n\
    Perf inc to algorithmic:    {perf[i]/perf[0]:6.5}")

    plt.savefig('perf_vs_input.eps', format='eps')
    plt.show()


def differentScenes():
    flop_counts_vect = np.array(list(get_flop_counts(FILENAMES_SCENES_FLOPS_VECT).values())).flatten()
    flop_counts_lto = np.array(list(get_flop_counts(FILENAMES_SCENES_FLOPS_LTO).values())).flatten()
    runtime_vect = np.squeeze(np.array(list(get_runtimes(FILENAMES_SCENES_RUNTIMES_VECT).values())))
    runtime_lto = np.squeeze(np.array(list(get_runtimes(FILENAMES_SCENES_RUNTIMES_LTO).values())))
    runtime_lto = [np.median(x) for x in runtime_lto]
    runtime_vect = [np.median(x) for x in runtime_vect]
    data_points_lto = [((elem/(runtime_lto[idx]/1000)) / (2.6 * 1e9))/ PEAK_PERF_VECTORIZED * 100 for idx,elem in enumerate(flop_counts_lto)]
    data_points_vect = [((elem/(runtime_vect[idx]/1000)) / (2.6 * 1e9))/PEAK_PERF_VECTORIZED * 100 for idx,elem in enumerate(flop_counts_vect)]


    # set the plot properties
    plt.style.use('seaborn')
    fig, ax = plt.subplots(1,1)
    fig.set_size_inches(10,6)
    ax.set_xscale('log', basex=2)

    # limit the axes and set the axis ticks
    plt.xlim([3.8, 67])
    plt.ylim([9, 46])
    x_axis = [4,8,16,32,64]
    plt.xticks(x_axis, fontsize=18, labels=["4", "8", "16", "32", "64"])
    plt.yticks(fontsize=18)
    x_axis = [4,8,16,32,64]

    # title, x,y-axis labels and grid
    plt.xlabel("Number of Objects per Shape Type in Scene [-]", fontsize=20)
    plt.ylabel("Fraction of Single Core Peak Performance [%]", fontsize=20, loc='top', rotation=0, labelpad=-437)
    plt.grid(b=None, which='major', axis='y', color='w')
    plt.grid(b=False, axis='x')
    plt.title(
        "Performance on Scenes of Different Sizes \nIntel Core i7-10750H @ 2.6 GHz, Memory @ 45.8 GB/s",
        {
            'verticalalignment': 'baseline',
            'horizontalalignment': 'left'
        }, 
        loc = 'left',
        pad = 40,
        fontsize = 20,
        fontweight = 'bold'
    )
    plt.tight_layout()
    print(data_points_lto)
    print(data_points_vect)

    # plot the actual data
    plt.plot(x_axis, data_points_lto, marker="P", color="olive")
    plt.plot(x_axis, data_points_vect, marker="o", color="darkgreen")
    plt.text(8, 34, "Vectorized", fontfamily="sans-serif", fontweight="bold", fontsize="16")
    plt.text(8, 17.5, "Mathematical", fontfamily="sans-serif", fontweight="bold", fontsize="16")
    plt.plot([11.5, 15.6], [34.7, 34.5], linestyle="dotted", color="black")
    plt.plot([12.5, 15.6], [18, 15.2], linestyle="dotted", color="black")

    plt.savefig("diffScenes.eps", format='eps')
    plt.show()

if __name__ == "__main__":
    runtime()
    roofline()
    perf_vs_input()
    differentScenes()