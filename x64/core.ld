ENTRY(main)
SECTIONS {
	. = 0x8000;
	/DISCARD/ : {
		*(.note.gnu.property .note.gnu.build-id .eh_frame .comment)
	}
}
