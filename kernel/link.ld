ENTRY(_start)

PHDRS {
	text PT_LOAD FLAGS(5);
	data PT_LOAD FLAGS(6);
}

SECTIONS {
	. = 0x8000;

	.text : {
		*(.text.entry)
		*(.text)
	} :text

	.data : {
		*(.data)
	} :data

	.bss : {
		*(.bss)
	} :data
}
