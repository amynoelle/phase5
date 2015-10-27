all: paper.pdf

countLooperBaseline.tex: countLooperBaseline buildSchedule
	grep MEASUREMENT countLooperBaseline | sed 's/MEASUREMENT//g' | sed 's/(#/ (PID /g' > tmp
	./buildSchedule tmp > countLooperBaseline.tex

twoCpuBound.tex: twoCpuBound buildSchedule
	grep MEASUREMENT twoCpuBound | sed 's/MEASUREMENT//g' | sed 's/(#/ (PID /g' > tmp
	./buildSchedule tmp > twoCpuBound.tex

paper.pdf: countLooperBaseline.tex twoCpuBound.tex states.tex paper.tex
	lualatex paper

clean:
	rm -f countLooperBaseline.tex
	rm -f twoCpuBound.tex
	rm -f paper.aux
	rm -f paper.log
	rm -f paper.pdf
	rm -f tmp
