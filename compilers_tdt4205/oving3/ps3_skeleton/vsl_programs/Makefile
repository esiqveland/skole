VSLC=../bin/vslc
ASFLAGS=-32
LDFLAGS=-m32
SOURCES=$(shell ls *.vsl)
ASSEMBLY=$(subst .vsl,.s,${SOURCES})
all: ${ASSEMBLY}
clean:
	@for FILE in ${ASSEMBLY}; do\
		if [ -e $$FILE ]; then \
			echo "Removing $$FILE" && rm $$FILE;\
		fi;\
	done
purge: clean
	@for FILE in ${TARGETS}; do\
		if [ -e $$FILE ]; then \
			echo "Removing $$FILE" && rm $$FILE;\
		fi;\
	done
%.s: %.vsl
	${VSLC} ${VSLFLAGS} -f $*.vsl -o $*.s
