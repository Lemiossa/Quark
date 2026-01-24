ENTRY(_START)

SECTIONS
{
	. = 0x8000;
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
