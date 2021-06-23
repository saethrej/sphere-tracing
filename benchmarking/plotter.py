import matplotlib.pyplot as plt
import numpy as np

FILENAMES_FLOPCOUNT = {
    #"base" : "finalBenchmarks/flop-count_scene34_base.csv",
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
    "lto_4" : "benccsvhmarks_scenechange/runtimes_scene31_lto.csv",
    "lto_8" : "benchmarks_scenechange/runtimes_scene32_lto.csv",
    "lto_12" : "benchmarks_scenechange/runtimes_scene33_lto.csv",
    "lto_16" : "benchmarks_scenechange/runtimes_scene34_lto.csv"
}

FILENAMES_SCENES_RUNTIMES_VECT = {
     
    "vect_4" : "benchmarks_scenechange/runtimes_scene31_vect.csv",
    "vect_8" : "benchmarks_scenechange/runtimes_scene32_vect.csv",
    "vect_12" : "benchmarks_scenechange/runtimes_scene33_vect.csv",
    "vect_16" : "benchmarks_scenechange/runtimes_scene34_vect.csv",

}

FILENAMES_SCENES_FLOPS_LTO = {
    "lto_4" : "benchmarks_scenechange/flop-count_scene31_lto.csv",
    "lto_8" : "benchmarks_scenechange/flop-count_scene32_lto.csv",
    "lto_12" : "benchmarks_scenechange/flop-count_scene33_lto.csv",
    "lto_16" : "benchmarks_scenechange/flop-count_scene34_lto.csv"
}

FILENAMES_SCENES_FLOPS_VECT = {
    "vect_4" : "benchmarks_scenechange/flop-count_scene31_vect.csv",
    "vect_8" : "benchmarks_scenechange/flop-count_scene32_vect.csv",
    "vect_12" : "benchmarks_scenechange/flop-count_scene33_vect.csv",
    "vect_16" : "benchmarks_scenechange/flop-count_scene34_vect.csv",
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
        "color" : "black",
        "marker" : "X"
    },
    {
        "name": "Algorithmic",
        "runtime" : "algo",
        "median" : [],
        "std" : [],
        "fmt" : "-D",
        "color" : "grey",
        "marker" : "D"
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
        "color" : "grey"
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
        "color" : "olive"
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
        "color" : "darkgreen"
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
        "color" : "brown"
    },
]

ROOFLINE_MAPPING = [
    #{
    #    "name": "Baseline",
    #    "runtime" : "base",
    #    "flopcount" : "base",
    #    "nr_bytes" : NR_BYTES_NON_VECTORIZED,
    #    "peak_perf" : PEAK_PERF_NON_VECTORIZED,
    #    "b_per_c" : BYTES_PER_CYCLES,
    #    "point_des" : "^",
    #    "color" : "black"
    #},
    {
        "name": "Algorithmic",
        "runtime" : "algo",
        "flopcount" : "algo",
        "nr_bytes" : NR_BYTES_NON_VECTORIZED,
        "peak_perf" : PEAK_PERF_NON_VECTORIZED,
        "b_per_c" : BYTES_PER_CYCLES,
        "point_des" : "P",
        "color" : "grey"
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
        "point_des" : "D",
        "color" : "olive"
    },
    {
        "name": "Vectorized",
        "runtime" : "vect",
        "flopcount" : "vect",
        "nr_bytes" : NR_BYTES_NON_VECTORIZED,
        "peak_perf" : PEAK_PERF_NON_VECTORIZED,
        "b_per_c" : BYTES_PER_CYCLES,
        "point_des" : "^",
        "color" : "darkgreen"
    },
    {
        "name": "Parallelized",
        "runtime" : "omp",
        "flopcount" : "vect",
        "nr_bytes" : NR_BYTES_NON_VECTORIZED,
        "peak_perf" : PEAK_PERF_NON_VECTORIZED,
        "b_per_c" : BYTES_PER_CYCLES,
        "point_des" : "X",
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
        "color" : "maroon"

    },
    {
        "name" : "Link-time optimized",
        "peak_perf" : PEAK_PERF_NON_VECTORIZED,
        "color" : "navy"

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
    plt.xlim([150, 2050])
    plt.ylim([0.006, 3000])
    plt.yticks(ticks=[0.01, 0.1, 1, 10, 100, 1000], labels=["0.01", "0.1", "1", "10", "100", "1000"])
    plt.xticks(ticks=input_sizes, labels=[200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 2000])

    # title, x,y-axis labels and grid
    plt.xlabel("Image Width [Pixels]", fontsize=14)
    plt.ylabel("Execution Time [s]", fontsize=14, loc='top', rotation=0, labelpad=-143)
    plt.grid(b=None, which='major', axis='x', color='w')
    plt.title(
        "Execution Times on Different Image Sizes \nIntel Core i7-10750H @ 2.6 GHz, Memory @ 45.8 GB/s",
        {
            'verticalalignment': 'baseline',
            'horizontalalignment': 'left'
        }, 
        loc = 'left',
        pad = 30,
        fontsize = 15,
        fontweight = 'bold'
    )
    plt.tight_layout()

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

        # plot the data without mapping names
        plt.plot(
            input_sizes, 
            mapping['median'], 
            marker=mapping["marker"],
            color=mapping['color'], 
            #label=mapping['name']
        )

    plt.savefig('runtime-plot.pdf', format='pdf')
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
        labels=[r'$\dfrac{1}{64}$', r'$\dfrac{1}{16}$', r'$\dfrac{1}{4}$', r"$1$", "4", "16", "64", "256", "1,024", "4,096", "16,384", "65,536"]
    )
    plt.yticks(
        ticks=[1/4, 1, 4, 16, 64],
        labels=[r'$\dfrac{1}{4}$', "1", "4", "16", "64"]
    )

    # title, x,y-axis labels and grid
    plt.xlabel("Operational Intensity [Flops/Byte]", fontsize=14)
    plt.ylabel("Performance [Flops/Cycle]", fontsize=14, loc='top', rotation=0, labelpad=-186)
    plt.grid(b=None, which='major', axis='x', color='w')
    #plt.grid(b=None, which='major', axis='y', color='w')
    plt.title(
        "Roofline Plot considering only Compulsory Misses \nIntel Core i7-10750H @ 2.6 GHz, Memory @ 45.8 GB/s",
        {
            'verticalalignment': 'baseline',
            'horizontalalignment': 'left'
        }, 
        loc = 'left',
        pad = 30,
        fontsize = 15,
        fontweight = 'bold'
    )
    plt.tight_layout()
    
    plt.plot(x_simd, y_simd, '--', linewidth=1.5, color='maroon')
    plt.plot(x_seq, y_seq, '-', linewidth=1.5, color='maroon')
    plt.plot(x_par, y_par, ':', linewidth=1.5, color='maroon')

    # plot actual data
    for mapping in ROOFLINE_MAPPING:
        runtime = runtimes[mapping['runtime']][-1]
        flopcount = flop_counts[mapping['flopcount']][-1]
        this_data = []
        for elem in runtime:
            this_data.append((flopcount) / (elem * 2.6 * 1e6))
        perf = np.median(this_data)
        intensity = flopcount / mapping['nr_bytes']
        plt.plot(intensity, perf,
                mapping['point_des'], color=mapping['color'], label=mapping['name'])

    plt.savefig('roofline.pdf', format='pdf')
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
    plt.ylim([0.7, 43])
    plt.yticks(ticks=[1,2,4,8,16,32], labels=["1", "2", "4", "8", "16", "32"])
    plt.xticks(ticks=input_sizes, labels=[200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 2000])

    # title, x,y-axis labels and grid
    plt.xlabel("Image Width [Pixels]", fontsize=14)
    plt.ylabel("Performance [GFlop/s]", fontsize=14, loc='top', rotation=0, labelpad=-160)
    plt.grid(b=None, which='major', axis='x', color='w')
    plt.title(
        "Performance on Different Image Sizes \nIntel Core i7-10750H @ 2.6 GHz, Memory @ 45.8 GB/s",
        {
            'verticalalignment': 'baseline',
            'horizontalalignment': 'left'
        }, 
        loc = 'left',
        pad = 30,
        fontsize = 15,
        fontweight = 'bold'
    )
    plt.tight_layout()    

    # plot the actual data
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

    plt.savefig('perf_vs_input.pdf', format='pdf')
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

    # limit the axes and set the axis ticks
    plt.xlim([3.5, 16.5])
    plt.ylim([6, 31.5])
    x_axis = [4,8,12,16]
    plt.xticks(x_axis)

    # title, x,y-axis labels and grid
    plt.xlabel("Number of Objects per Shape Type in Scene [-]", fontsize=14)
    plt.ylabel("Fraction of Single Core Peak Performance [%]", fontsize=14, loc='top', rotation=0, labelpad=-308)
    plt.grid(b=None, which='major', axis='x', color='w')
    #plt.grid(b=None, which='major', axis='y', color='w')
    plt.title(
        "Performance on Scenes of Different Sizes \nIntel Core i7-10750H @ 2.6 GHz, Memory @ 45.8 GB/s",
        {
            'verticalalignment': 'baseline',
            'horizontalalignment': 'left'
        }, 
        loc = 'left',
        pad = 30,
        fontsize = 15,
        fontweight = 'bold'
    )
    plt.tight_layout()

    # plot the actual data
    plt.plot(x_axis, data_points_lto, marker='D', color='olive')
    plt.plot(x_axis, data_points_vect, marker='D', color='darkgreen')

    plt.savefig("diffScenes.pdf", format='pdf')
    plt.show()

if __name__ == "__main__":
    runtime()
    roofline()
    perf_vs_input()
    #differentScenes()