

		x := 0
loop0:	
		t1 := x < n
		fjump t1 endloop0

		y := 0
loop1:	
		t3 := n - 1
		t2 := y < t3
		fjump t2 endloop1
		
		t4 := y + 1
		t5 := array[y] > array[t4]
		fjump t5 endif0
		temp := array[t4]
		array[t4] := array[y]
		array[y] := temp
endif0:
		y := y + 1
		jump loop1
endloop1:
		x := x + 1
		jump loop0
endloop0:


/* vim: set ts=4 sw=4: */
