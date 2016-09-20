#!/bin/bash
#SBATCH -J lock_compare # job name
#SBATCH -o result-tacc-%j.txt # output and error file name (%j=jobID)
#SBATCH -n 16 # total number of cpus requested
#SBATCH -p development # queue -- normal, development, etc.
#SBATCH -t 02:00:00 # run time (hh:mm:ss) - 2.0 hours
#SBATCH --mail-user=siming.liu@utdallas.edu
#SBATCH --mail-type=begin # email me when the job starts
#SBATCH --mail-type=end # email me when the job finishes

${HOME}/multicore_project_1/src/lock_compare 16 1000000 16

