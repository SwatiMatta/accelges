DESCRIPTION = "Openmoko Gestures"
SECTION = "openmoko/utilities"

PN = "gestures"
PV = "0.1.0+svnr${SRCREV}"
PR = "r0"

SRC_URI = "svn://svn.projects.openmoko.org/svnroot;module=gestures;proto=https"

S = "${WORKDIR}/gestures"

inherit autotools

