dist_man1_MANS = snowmonkey.1

BUILT_SOURCES = snowmonkey.1

snowmonkey.1: snowmonkey.1.pod
	@echo "Generating "$@
	@cat snowmonkey.1.pod | pod2man --section=1 --release=snowmonkey-$(SM_VERSION) --name=snowmonkey --center="snowmonkey-$(SM_VERSION) man page" > snowmonkey.1

CLEANFILES=snowmonkey.1 

# TODO TODO TODO ! FIXME FIXME FIXME (pas de fichier .pod de copié :()
