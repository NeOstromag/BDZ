LIBS =	\
	suricata/__init__.py \
	suricata/update/commands/__init__.py \
	suricata/update/commands/addsource.py \
	suricata/update/commands/checkversions.py \
	suricata/update/commands/disablesource.py \
	suricata/update/commands/enablesource.py \
	suricata/update/commands/listsources.py \
	suricata/update/commands/removesource.py \
	suricata/update/commands/updatesources.py \
	suricata/update/compat/__init__.py \
	suricata/update/compat/ordereddict.py \
	suricata/update/compat/argparse/__init__.py \
	suricata/update/compat/argparse/argparse.py \
	suricata/update/configs/__init__.py \
	suricata/update/config.py \
	suricata/update/data/__init__.py \
	suricata/update/data/index.py \
	suricata/update/data/update.py \
	suricata/update/__init__.py \
	suricata/update/engine.py \
	suricata/update/exceptions.py \
	suricata/update/extract.py \
	suricata/update/loghandler.py \
	suricata/update/main.py \
	suricata/update/maps.py \
	suricata/update/matchers.py \
	suricata/update/net.py \
	suricata/update/notes.py \
	suricata/update/osinfo.py \
	suricata/update/parsers.py \
	suricata/update/rule.py \
	suricata/update/sources.py \
	suricata/update/util.py \
	suricata/update/version.py

CONFIGS = \
	suricata/update/configs/disable.conf \
	suricata/update/configs/drop.conf \
	suricata/update/configs/enable.conf \
	suricata/update/configs/modify.conf \
	suricata/update/configs/threshold.in \
	suricata/update/configs/update.yaml

BINS =	suricata-update

if HAVE_PYTHON

install-exec-local:
	install -d -m 0755 "$(DESTDIR)$(prefix)/lib/suricata/python/suricata/update/commands"
	install -d -m 0755 "$(DESTDIR)$(prefix)/lib/suricata/python/suricata/update/compat/argparse"
	install -d -m 0755 "$(DESTDIR)$(prefix)/lib/suricata/python/suricata/update/configs"
	install -d -m 0755 "$(DESTDIR)$(prefix)/lib/suricata/python/suricata/update/data"
	for lib in $(LIBS); do \
		install $(srcdir)/$$lib "$(DESTDIR)$(prefix)/lib/suricata/python/$$lib"; \
	done
	for config in $(CONFIGS); do \
		install -m 0644 $(srcdir)/$$config "$(DESTDIR)$(prefix)/lib/suricata/python/$$config"; \
	done
	for bin in $(BINS); do \
		cat "$(srcdir)/bin/$$bin" | \
		    sed -e "1 s,.*,#"'!'" ${HAVE_PYTHON}," > "${DESTDIR}$(bindir)/$$bin"; \
		chmod 0755 "$(DESTDIR)$(bindir)/$$bin"; \
	done

uninstall-local:
	rm -f $(DESTDIR)$(bindir)/suricata-update
	rm -rf $(DESTDIR)$(prefix)/lib/suricata/python

clean-local:
	find . -name \*.pyc -print0 | xargs -0 rm -f

endif
