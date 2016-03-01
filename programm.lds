ENTRY(main)
SECTIONS
{
	. = 0x00008000;
	.text.startup : { *(.text.startup) }
	.text : { *(.text)}
}

