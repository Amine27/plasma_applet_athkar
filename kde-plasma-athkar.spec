Name:           kde-plasma-athkar
Version:        0.2.4
Release:        2%{?dist}
Summary:        Athkar is a plasmoid that display Islamic supplications every predetermined time from wathakker.net or local database

Group:          User Interface/Desktops
License:        GPLv2+
URL:            http://git.ojuba.org/cgit/plasma-widget-athkar/about/
%define         upstream_name plasma-widget-athkar
#%%define         upstream_version %{version}
%define         upstream_version master
Source0:        http://git.ojuba.org/cgit/%{upstream_name}/snapshot/%{upstream_name}-%{upstream_version}.tar.bz2

BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Requires:       monajat-database
BuildRequires:  kdebase-workspace-devel >= 4.2.0
BuildRequires:  gettext

%description
Athkar is a plasmoid that display Islamic supplications every predetermined time from wathakker.net or local database

%prep
%setup -qn %{upstream_name}-%{upstream_version}


%build
mkdir -p %{_target_platform}
pushd %{_target_platform}
%{cmake_kde4} ..
popd

make VERBOSE=1 %{?_smp_mflags} -C %{_target_platform}


%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot} -C %{_target_platform}
rm %{buildroot}/%{_kde4_datadir}/kde4/apps/plasma-athkar/data.db
ln -s /usr/share/monajat/data.db %{buildroot}/%{_kde4_datadir}/kde4/apps/plasma-athkar/data.db

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%doc COPYING README
%{_kde4_libdir}/kde4/plasma_applet_athkar.so
%{_kde4_datadir}/kde4/services/plasma_applet_athkar.desktop
%{_kde4_datadir}/kde4/apps/plasma-athkar/*
%{_datadir}/locale/*/*/*.mo

%changelog
* Tue Sep 22 2009 Muayyad Saleh Alsadi <alsadi@ojuba.org> 0.0.1-1
- Initial package

