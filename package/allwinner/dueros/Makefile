##############################################
# OpenWrt Makefile for smartlinkd program
#
#
# Most of the variables used here are defined in
# the include directives below. We just need to
# specify a basic description of the package,
# where to build our program, where to find
# the source files, and where to install the
# compiled program on the router.
#
# Be very careful of spacing in this file.
# Indents should be tabs, not spaces, and
# there should be no trailing whitespace in
# lines that are not commented.
#
##############################################
include $(TOPDIR)/rules.mk

# Name and release number of this package
PKG_NAME:=dueros
PKG_VERSION:=1
PKG_RELEASE:=1


PKG_BUILD_DIR := $(COMPILE_DIR)/$(PKG_NAME)
HAVE_SOURCE := yes

include $(BUILD_DIR)/package.mk

define Package/$(PKG_NAME)/Default
  SECTION:=utils
  CATEGORY:=Allwinner
  DEPENDS:=  
#+libpthread +libmad +alsa-lib
endef

define Package/$(PKG_NAME)
  $(call Package/$(PKG_NAME)/Default)
  TITLE:=dueros app
endef

define Package/$(PKG_NAME)/description
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/
	$(CP) -r ./src $(PKG_BUILD_DIR)/
endef

define Build/Compile

	$(MAKE) -C $(PKG_BUILD_DIR)/src \
		MAKE="$(MAKE)" \
		ARCH="$(TARGET_ARCH)" \
		AR="$(TARGET_AR)" \
		CC="$(TARGET_CC)" \
		CXX="$(TARGET_CXX)" \
		CFLAGS="$(TARGET_CFLAGS)"\
		LDFLAGS="$(TARGET_LDFLAGS)" \
		CONFIG_PREFIX="$(PKG_INSTALL_DIR)" \
		all

endef
ifeq (0,1)
define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_DIR) $(1)/home
	$(INSTALL_DIR) $(1)/home/asound
#	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/lib/*.so $(1)usr/lib/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/sbin/* $(1)/sbin/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/home/*.* $(1)/home/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/home/asound/* $(1)/home/asound/
endef

endif

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_DIR) $(1)/home
	$(INSTALL_DIR) $(1)/home/asound
	#$(INSTALL_DATA) $(PKG_BUILD_DIR)/src/lib/* $(1)/usr/lib/ 
	#$(INSTALL_DATA) $(PKG_BUILD_DIR)/src/alsa/install/lib/libasound* $(1)/usr/lib/ 
	#$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/sbin/* $(1)/sbin/
	#$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/config/*.* $(1)/home/
	#$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/config/asound/* $(1)/home/asound/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/home/*  $(1)/home/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
