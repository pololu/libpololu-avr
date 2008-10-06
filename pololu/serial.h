void serial_set_baud_rate(unsigned long baud);
void serial_receive(char *buffer, unsigned char size);
unsigned char serial_get_received_bytes();
char serial_receive_buffer_full();
void serial_send(char *buffer, unsigned char size);
unsigned char serial_get_sent_bytes();
char serial_send_buffer_empty();
