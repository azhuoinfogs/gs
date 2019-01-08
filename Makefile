cc = gcc  --std=c99
prom = test_sst
obj =  block_builder.o util.o table_builder.o
$(prom): $(obj)
	$(cc) -o $(prom) $(obj)
%.o: %.c $(deps)
	$(cc) -c $< -o $@
clean:
	rm -rf *.o leveldb* test_sst
