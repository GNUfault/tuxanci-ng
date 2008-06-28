
all: widget_button.o widget_buttonimage.o widget_check.o widget_label.o widget_select.o widget_textfield.o widget_image.o

widget_button.o: widget_button.c widget_button.h
	$(CC) $(CFLAGS) -o widget_button.o -c widget_button.c

widget_buttonimage.o: widget_buttonimage.c widget_buttonimage.h
	$(CC) $(CFLAGS) -o widget_buttonimage.o -c widget_buttonimage.c

widget_check.o: widget_check.c widget_check.h
	$(CC) $(CFLAGS) -o widget_check.o -c widget_check.c

widget_image.o: widget_image.c widget_image.h
	$(CC) $(CFLAGS) -o widget_image.o -c widget_image.c

widget_label.o: widget_label.c widget_label.h
	$(CC) $(CFLAGS) -o widget_label.o -c widget_label.c

widget_select.o: widget_select.c widget_select.h
	$(CC) $(CFLAGS) -o widget_select.o -c widget_select.c

widget_textfield.o: widget_textfield.c widget_textfield.h
	$(CC) $(CFLAGS) -o widget_textfield.o -c widget_textfield.c

clean:
	rm -rf *.o


