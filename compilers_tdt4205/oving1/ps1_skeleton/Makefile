CFLAGS+=-std=c99
TARGETS=parens
all: ${TARGETS}
test: all
	@echo ""
	@echo "Testing balanced file... this should be ok"
	cat balanced.c | ./parens
	@echo ""
	@echo "Testing imbalanced file... this should cause a failure"
	cat imbalanced.c | ./parens
	@echo ""
	@echo "Testing imba2 file... this should cause a failure"
	cat imba2.c | ./parens
purge:
	-rm -f ${TARGETS}
