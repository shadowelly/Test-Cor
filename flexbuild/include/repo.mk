#####################################
#
# Copyright 2017 NXP
#
#####################################

SHELL=/bin/bash

define fetch-git-tree
	@if [ ! -d $1 ]; then \
		tree=`echo $1 | sed 's/-/_/g'` && \
		branch=`grep -E "^$${tree}_branch" $(TOPDIR)/configs/$(CONFIGLIST) | cut -d= -f2` && \
		tag=`grep -E "^$${tree}_tag" $(TOPDIR)/configs/$(CONFIGLIST) | cut -d= -f2` && \
		commit=`grep -E "^$${tree}_commit" $(TOPDIR)/configs/$(CONFIGLIST) | cut -d= -f2` && \
		repourl=`grep -E "^$${tree}_url" $(TOPDIR)/configs/$(CONFIGLIST) | cut -d= -f2` && \
		if [ -z "$$repourl" ]; then repourl=$(GIT_REPOSITORY_URL)/$1; fi && \
		if [ -n "$$tag" ]; then git clone $$repourl && cd `realpath $(TOPDIR)/packages/*/$1` && git checkout $$tag -b $$tag && cd -; \
		elif [ -n "$$commit" ]; then git clone $$repourl && cd `realpath $(TOPDIR)/packages/*/$1` && git checkout $$commit -b $$commit && cd -; \
		elif [ -n "$$branch" ]; then git clone $$repourl -b $$branch; fi;\
	fi
endef


define repo-update
	@for tree in $2; do \
	    echo -e "\nrepo: $$tree"; tree2=`echo $$tree | sed 's/-/_/g'`; \
	    branch=`grep -E "^$${tree2}_branch" $(TOPDIR)/configs/$(CONFIGLIST) | cut -d= -f2` && if [ -n "$$branch" ]; then echo branch = $$branch; fi; \
	    commit=`grep -E "^$${tree2}_commit" $(TOPDIR)/configs/$(CONFIGLIST) | cut -d= -f2` && if [ -n "$$commit" ]; then echo commit = $$commit; fi; \
	    tag=`grep -E "^$${tree2}_tag" $(TOPDIR)/configs/$(CONFIGLIST) | cut -d= -f2` && if [ -n "$$tag" ]; then echo tag = $$tag; fi; \
	    repourl=`grep -E "^$${tree2}_url" $(TOPDIR)/configs/$(CONFIGLIST) | cut -d= -f2` && if [ -z "$$repourl" ]; then repourl=$(GIT_REPOSITORY_URL)/$$tree; fi ; \
	    if [ -d $$tree ]; then \
		if [ $1 = update -a -n "$$branch" ]; then cd `realpath $(TOPDIR)/packages/*/$$tree` && if [ "`cat .git/HEAD | cut -d/ -f3`" != "$$branch" ]; then \
		    if git show-ref --verify --quiet refs/heads/$$branch; then git checkout $$branch && git pull origin $$branch; \
		    else git checkout remotes/origin/$$branch -b $$branch;fi;fi || exit 1; cd -; \
		elif [ $1 = update -a -z "$$branch" -a -n "$$tag" ]; then cd `realpath $(TOPDIR)/packages/*/$$tree` && if ! git show-ref --verify --quiet refs/tags/$$tag; \
		    then git pull||true;fi && if [ "`cat .git/HEAD | cut -d/ -f3`" != "$$tag" ]; then \
		    if git show-ref --verify --quiet refs/heads/$$tag; then git checkout $$tag; else git checkout $$tag -b $$tag;fi;fi || exit 1; cd -; \
		elif [ $1 = branch -a -n "$$branch" ]; then cd `realpath $(TOPDIR)/packages/*/$$tree` && git checkout $$branch || exit 1; cd -; \
		elif [ $1 = tag -a -n "$$tag" ]; then cd `realpath $(TOPDIR)/packages/*/$$tree` && if ! git show-ref --verify --quiet refs/tags/$$tag; then \
		    git fetch --tags || true;fi && if [ "`cat .git/HEAD | cut -d/ -f3`" != "$$tag" ]; then if git show-ref --verify --quiet refs/heads/$$tag; \
		    then git checkout $$tag; else git checkout $$tag -b $$tag;fi;fi || exit 1; cd -; \
		elif [ $1 = commit -a -n "$$commit" ]; then cd `realpath $(TOPDIR)/packages/*/$$tree` && git checkout $$commit || exit 1; cd -; \
		elif [ $1 = fetch ]; then echo $$tree exists!; \
		else echo unsupported as branch/commit info is not specified in $(CONFIGLIST)!; fi;\
	    elif [ $1 = fetch ]; then \
		if [ -n "$$tag" ]; then git clone $$repourl && cd `realpath $(TOPDIR)/packages/*/$$tree` && git checkout $$tag -b $$tag && cd -; \
		elif [ -n "$$commit" ]; then git clone $$repourl && cd `realpath $(TOPDIR)/packages/*/$$tree` && git checkout $$commit -b $$commit && cd -; \
		elif [ -n "$$branch" ]; then git clone $$repourl -b $$branch; if [ -d code -a ! -d crconf ]; then mv code crconf; fi; fi;\
	    fi; \
	done
endef
