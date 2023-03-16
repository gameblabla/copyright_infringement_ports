
// Resource Table
// Created by IDE, Do not modify this table

.TEXT
.public _RES_Table;
.external __RES_FONT_BIN_sa
.public _RES_FONT_BIN_SA;
.external __RES_FONT_BIN_ea;
.public _RES_FONT_BIN_EA;
.external __RES_FRAMES_BIN_sa
.public _RES_FRAMES_BIN_SA;
.external __RES_FRAMES_BIN_ea;
.public _RES_FRAMES_BIN_EA;
.external __RES_FRAMES_PAL_BIN_sa
.public _RES_FRAMES_PAL_BIN_SA;
.external __RES_FRAMES_PAL_BIN_ea;
.public _RES_FRAMES_PAL_BIN_EA;
.external __RES_TITLE_BIN_sa
.public _RES_TITLE_BIN_SA;
.external __RES_TITLE_BIN_ea;
.public _RES_TITLE_BIN_EA;
.external __RES_TITLE_PAL_BIN_sa
.public _RES_TITLE_PAL_BIN_SA;
.external __RES_TITLE_PAL_BIN_ea;
.public _RES_TITLE_PAL_BIN_EA;
.external __RES_YAMI_BIN_sa
.public _RES_YAMI_BIN_SA;
.external __RES_YAMI_BIN_ea;
.public _RES_YAMI_BIN_EA;
.external __RES_YAMI_PAL_BIN_sa
.public _RES_YAMI_PAL_BIN_SA;
.external __RES_YAMI_PAL_BIN_ea;
.public _RES_YAMI_PAL_BIN_EA;
.external __RES_BLACK_BIN_sa
.public _RES_BLACK_BIN_SA;
.external __RES_BLACK_BIN_ea;
.public _RES_BLACK_BIN_EA;


_RES_Table:

_RES_FONT_BIN_SA:
	.DW offset __RES_FONT_BIN_sa,seg __RES_FONT_BIN_sa;
_RES_FONT_BIN_EA:
	.DW offset __RES_FONT_BIN_ea,seg __RES_FONT_BIN_ea;

_RES_FRAMES_BIN_SA:
	.DW offset __RES_FRAMES_BIN_sa,seg __RES_FRAMES_BIN_sa;
_RES_FRAMES_BIN_EA:
	.DW offset __RES_FRAMES_BIN_ea,seg __RES_FRAMES_BIN_ea;

_RES_FRAMES_PAL_BIN_SA:
	.DW offset __RES_FRAMES_PAL_BIN_sa,seg __RES_FRAMES_PAL_BIN_sa;
_RES_FRAMES_PAL_BIN_EA:
	.DW offset __RES_FRAMES_PAL_BIN_ea,seg __RES_FRAMES_PAL_BIN_ea;

_RES_TITLE_BIN_SA:
	.DW offset __RES_TITLE_BIN_sa,seg __RES_TITLE_BIN_sa;
_RES_TITLE_BIN_EA:
	.DW offset __RES_TITLE_BIN_ea,seg __RES_TITLE_BIN_ea;

_RES_TITLE_PAL_BIN_SA:
	.DW offset __RES_TITLE_PAL_BIN_sa,seg __RES_TITLE_PAL_BIN_sa;
_RES_TITLE_PAL_BIN_EA:
	.DW offset __RES_TITLE_PAL_BIN_ea,seg __RES_TITLE_PAL_BIN_ea;

_RES_YAMI_BIN_SA:
	.DW offset __RES_YAMI_BIN_sa,seg __RES_YAMI_BIN_sa;
_RES_YAMI_BIN_EA:
	.DW offset __RES_YAMI_BIN_ea,seg __RES_YAMI_BIN_ea;

_RES_YAMI_PAL_BIN_SA:
	.DW offset __RES_YAMI_PAL_BIN_sa,seg __RES_YAMI_PAL_BIN_sa;
_RES_YAMI_PAL_BIN_EA:
	.DW offset __RES_YAMI_PAL_BIN_ea,seg __RES_YAMI_PAL_BIN_ea;

_RES_BLACK_BIN_SA:
	.DW offset __RES_BLACK_BIN_sa,seg __RES_BLACK_BIN_sa;
_RES_BLACK_BIN_EA:
	.DW offset __RES_BLACK_BIN_ea,seg __RES_BLACK_BIN_ea;


// End Table
