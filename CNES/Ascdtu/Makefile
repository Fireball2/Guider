###############################################################################
# MAKEFILE - APP ASCDTU
# Version	:	1.0.0
# Date		:	30/11/12
# Author	:	RTH-CIO
###############################################################################
# VERSION HISTORY:
#	- 1.0.0 : Create
###############################################################################

#------------------------------------------------------------------------------
# Makefile Parameter
#------------------------------------------------------------------------------
# Current Folder
ifndef TOPDIR 
TOPDIR		= ../../../..
export TOPDIR
endif

# Name of application
APPLICATION_NAME=ascdtu

# Install directory
INSTALLDIR=$(TOPDIR)/../configuration/install/brd/lab/lab/drivers

# Documentation directory
DOCUMENTATIONDIR=${TOPDIR}/SenseurStellaireDTU/Brd/Doc/${APPLICATION_NAME}

# LAB rules
include $(TOPDIR)/../configuration/rules.mk

# Library path
STOCKAGE=$(TOPDIR)/StockageRejeu/Brd/App/Stockage

# Linker include paths
EXTRA_INC += $(SYSLAY_INC_PATH) -I$(STOCKAGE)

# Linker library paths
EXTRA_LIBS += $(SYSLAY_LIBS_PATH)

# Linker library to link
EXTRA_LIBS += $(SYSLAY_LIBS)

# Specific C++ flags
CPPFLAGS+= ${EXTRA_INC}
LDCPPFLAGS += 
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Makefile specific rules
#------------------------------------------------------------------------------
.PHONY: all clean install doxygen doxygenclean

# Make binary
all: ${APPLICATION_NAME}
${APPLICATION_NAME}: $(OBJS_CPP)
	@$(ECHO)  "\tLINKING"  Application ${APPLICATION_NAME}...
	@mkdir -p ${dir $@}
	$(Q)$(CPP) $(LDCPPFLAGS) -o $@  $^  ${EXTRA_LIBS}

# Create depend of C (.c)
${DEP_FILE}: ${DEPS_CPP}
	@${ECHO} "\tMAKING DEPEND FILE" $@
	${Q}${MAKEDIR} ${DEPDIR}; \
	${RM} ${DEP_FILE}; \
	for dep in $^ ; do cat $$dep >>  ${DEP_FILE}; done;

# Installation in the install directory (INSTALLDIR), file copy		
install:
	@$(ECHO) "\tINSTALL " Application ${APPLICATION_NAME}...
	${Q}${MAKEDIR} ${INSTALLDIR}
	$(Q)cp ${APPLICATION_NAME} ${INSTALLDIR}/${APPLICATION_NAME}

# Remove library and object files
clean:
	@${ECHO} "\tCLEANING ALL"..
	${Q}${RM} ${APPLICATION_NAME}
	${Q}${RM} *.o *core *.so
	${Q}${RM} -r ${OBJDIR}
	${Q}${RM} -r ${DEPDIR}

# Create doxygen hmtl
doxygen:
	${Q}${MAKEDIR} ${DOCUMENTATIONDIR}
	${Q} ${DOXYGEN} ${DOCUMENTATIONDIR}/doxyfile

# Erase doxygen hmtl
doxygenclean:
	${RM} -r ${DOCUMENTATIONDIR}/doxygen

ifeq (${MAKECMDGOALS}, all)
-include ${DEP_FILE}
endif