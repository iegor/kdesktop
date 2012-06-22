## (C) 1997 Stephan Kulow

AUTOMAKE_OPTIONS = foreign 
DISTCLEANFILES = inst-apps

install-data-local:
	@echo ""
	@echo ""
	@echo "*************** Important *************************"
	@echo ""
	@echo "This module contains unreleased software."
	@echo ""
	@echo "The software may compile and work, but it may just"
	@echo "as well neither compile nor work."
	@echo ""
	@echo "****************************************************"
	@echo ""

include admin/deps.am


SUBDIRS=$(TOPSUBDIRS)
