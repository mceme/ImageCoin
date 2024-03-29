package=freetype
$(package)_version=2.10.4
$(package)_download_path=https://download.savannah.gnu.org/releases/freetype/
$(package)_file_name=$(package)-$($(package)_version).tar.xz
$(package)_sha256_hash=86a854d8905b19698bbc8f23b860bc104246ce4854dcea8e3b0fb21284f75784

define $(package)_set_vars
  $(package)_config_opts=--without-zlib --without-png --disable-static
  $(package)_config_opts_linux=--with-pic
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef
