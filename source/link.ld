ENTRY(_start)

PHDRS {
	text PT_LOAD FLAGS(5);
	data PT_LOAD FLAGS(6);
}

SECTIONS {
	. = 0x7e00;

	.text : {
		*(.text.entry)
		*(.text)
	} :text

	.data : {
		*(.data)
	} :data

	. = 0x100000;
	bss_start = .;
	.bss : {
		*(.bss)
	} :data
	bss_end = .;
}
