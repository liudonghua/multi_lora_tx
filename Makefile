#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=loratx
#PKG_RELEASE:=
PKG_VERSION:=1.0

PKG_MAINTAINER:=liudonghua <shaphone2009@gmail.com>
PKG_LICENSE:=GPL-2.0+


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

PKG_INSTALL=

TARGET_CXXFLAGS+= -O2

define Package/loratx
	SECTION:=utils
	CATEGORY:=Extra packages
	TITLE:=loratx
	DEPENDS:= +boost +boost-date_time +boost-thread +boost-chrono +boost-system +jsoncpp +libpthread
endef

define Package/loratx/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/loratx $(1)/usr/sbin/loratx
	$(INSTALL_BIN) ./files/loratx.sh $(1)/usr/sbin/loratx.sh
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/conf.json $(1)/etc/conf.json
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/loratx.init $(1)/etc/init.d/loratx
endef

$(eval $(call BuildPackage,loratx))
