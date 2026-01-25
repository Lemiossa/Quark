ENTRY(_START)

SECTIONS
{
	. = 0x100000;
	.text : {
		*(.text.entry)
		*(.text)
	}

	.data : {
		*(.data)
	}

	.bss : {
		*(.bss)
	}
}
