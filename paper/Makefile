all: paper.pdf

countLooperBaseline.tex: countLooperBaseline buildSchedule
	grep MEASUREMENT countLooperBaseline | sed 's/MEASUREMENT//g' | sed 's/(#/ (PID /g' > tmp
	./buildSchedule tmp > countLooperBaseline.tex

catLooperBaseline.tex: catLooperBaseline buildSchedule
	grep MEASUREMENT catLooperBaseline | sed 's/MEASUREMENT//g' | sed 's/(#/ (PID /g' > tmp
	./buildSchedule tmp > catLooperBaseline.tex

twoCpuBound.tex: twoCpuBound buildSchedule
	grep MEASUREMENT twoCpuBound | sed 's/MEASUREMENT//g' | sed 's/(#/ (PID /g' > tmp
	./buildSchedule tmp > twoCpuBound.tex

niceCpuBound.tex: niceCpuBound buildSchedule
	grep MEASUREMENT niceCpuBound | sed 's/MEASUREMENT//g' | sed 's/(#/ (PID /g' > tmp
	./buildSchedule tmp > niceCpuBound.tex

oneEach.tex: oneEach buildSchedule
	grep MEASUREMENT oneEach | sed 's/MEASUREMENT//g' | sed 's/(#/ (PID /g' > tmp
	./buildSchedule tmp > oneEach.tex

paper.pdf: countLooperBaseline.tex catLooperBaseline.tex twoCpuBound.tex niceCpuBound.tex oneEach.tex states.tex paper.tex
	lualatex paper

clean:
	rm -f countLooperBaseline.tex
	rm -f catLooperBaseline.tex
	rm -f twoCpuBound.tex
	rm -f niceCpuBound.tex
	rm -f oneEach.tex
	rm -f paper.aux
	rm -f paper.log
	rm -f paper.pdf
	rm -f tmp
