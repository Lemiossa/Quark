ENTRY(_start)

PHDRS {
	text PT_LOAD FLAGS(5);
}

SECTIONS {
	. = 0x7c00;

	.text : {
		*(.text)
	} :text
}
