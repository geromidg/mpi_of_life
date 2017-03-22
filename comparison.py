#!/usr/bin/python

from sys import	argv

import numpy as np
from matplotlib import pyplot as plt

plt.style.use('ggplot')

def main(results_filename):
	with open(results_filename, 'r') as file:
		lines = file.read().splitlines()

	results = [line.split() for line in lines]

	processes = [int(result[0]) for result in results]
	serial_times = np.array([float(result[1]) for result in results])
	parallel_times = np.array([float(result[2]) for result in results])

	figure = plt.figure(figsize=(8, 6), dpi=80, facecolor='w', edgecolor='k')
	axes = figure.add_subplot(1, 1, 1)

	ind = np.arange(3)
	width = 0.25

	serial_bars = axes.bar(ind + width * 0, serial_times, width, color='k', alpha=0.6)
	parallel_bars = axes.bar(ind + width * 1.02, parallel_times, width)
	# axes.bar(ind + width * 0, serial_times * processes[::-1], width, alpha=0.2)
	# axes.bar(ind + width * 1.02, parallel_times * processes[::-1], width, alpha=0.2)
	
	axes.set_title('GAME OF LIFE', {'weight': 'bold', 'size': 20})
	axes.title.set_position([.5, 1.06])
	axes.set_xlabel('boards (processes)', {'size': 15})
	axes.set_ylabel('execution time', {'size': 15})
	axes.set_xticks(ind + width * 1.02)
	axes.set_xticklabels(processes)
	axes.margins(0.05)
	axes.legend((serial_bars[0], parallel_bars[0]), ('Serial', 'Parallel'), title='Implementation', loc='best', fontsize='small', shadow=True, framealpha=0.5)

	for bars in [serial_bars, parallel_bars]:
		for rect in bars:
			height = rect.get_height()
			axes.text(rect.get_x() + rect.get_width() / 2., 1.0 * height, '%.2f' % height, ha='center', va='bottom')

	figure.tight_layout()
	figure.savefig('results.png', bbox_inches='tight')
	# plt.show()

if __name__ == '__main__':
	if len(argv) != 2:
		print 'Usage: %s [results_filename].txt' % (argv[0])
		exit(-1)

	main(argv[1])