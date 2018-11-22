#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=zigbeeservice
PKG_VERSION:=0.6
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)

PKG_LICENSE:=GPLv2 GPLv2+
PKG_LICENSE_FILES:=

include $(BUILD_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=Allwinner
  TITLE:=IMIO Circle Gateway zigbeeservice
  DEPENDS:= +libblobmsg-json +libcares +libcurl +libjson-c +liblua +libmosquitto \
                      +libnghttp2 +libsqlite3 +libubox +hostapd +ca-certificates \
                      +libubus +libuci +libmbedtls +libustream-cyassl #+libssl #+libcrypto 
endef

define Package/$(PKG_NAME)/description
	Connect to IMIO Cloud & cap 
	This package is a bridge connectting a Zigbee device through RS-232 serial port
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) -r ./src $(PKG_BUILD_DIR)/
	$(CP) -r ./file $(PKG_BUILD_DIR)/
endef

define Build/Configure
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

define Package/$(PKG_NAME)/install
	#/sbin/
	$(INSTALL_DIR) $(1)/sbin/
	$(CP) $(PKG_BUILD_DIR)/file/sbin/* $(1)/sbin/
	#/etc/
	$(INSTALL_DIR) $(1)/etc/
	$(CP) $(PKG_BUILD_DIR)/file/etc/* $(1)/etc/
	$(CP) $(PKG_BUILD_DIR)/file/rc.local $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/config/
	#/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/file/lib/* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/libshuncomzigbee.so $(1)/usr/lib/
	#/usr/sbin/
	#$(INSTALL_DIR) $(1)/usr/sbin/
	#$(CP) $(PKG_BUILD_DIR)/file/usr/sbin/* $(1)/usr/sbin/
	#/home/
	$(INSTALL_DIR) $(1)/home/
	$(CP) $(PKG_BUILD_DIR)/file/home/* $(1)/home/
	$(CP) $(PKG_BUILD_DIR)/file/zigbeeserviceStart.sh $(1)/home/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/zigbeeService $(1)/home/
	#/home/asound/
	$(INSTALL_DIR) $(1)/home/asound/
	$(CP) $(PKG_BUILD_DIR)/file/home/asound/* $(1)/home/asound/
	#/home/log/
	$(INSTALL_DIR) $(1)/home/log/
	
endef

$(eval $(call BuildPackage,zigbeeservice))
