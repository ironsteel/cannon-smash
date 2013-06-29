
Summary:	CannonSmash is a 3D tabletennis game.
Name:		csmash
Version:	0.6.5
Release:	1
Group:		Games/Sports
Copyright:	GPL
Source:		%{name}-%{version}.tar.gz
Patch0:		csmash-0.6.5-loadparts.patch
URL:		http://cannonsmash.sourceforge.net/
Packager:	Victor Soroka <gbs@tnss.kharkov.ua>
BuildPreReq:	/usr/include/GL/gl.h
BuildRoot:	%{_tmppath}/%{name}-%{version}-buildroot

%description
CannonSmash is a 3D tabletennis game. 
The goal of this project is to represent various strategy 
of tabletennis on computer game.

%prep
%setup -q
%patch0

%build
CFLAGS=$RPM_OPT_FLAGS
%configure
make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install
mkdir -p $RPM_BUILD_ROOT%{_datadir}/gnome/apps/Games
cat > $RPM_BUILD_ROOT%{_datadir}/gnome/apps/Games/%{name}.desktop << __EOF__
[Desktop Entry]
Name=CannonSmash
Comment=3D tabletennis game
Exec=%{_prefix}/bin/%{name}
Terminal=0
Type=Applications
__EOF__

strip -s $RPM_BUILD_ROOT%{_bindir}/%{name}


%clean
rm -rf $RPM_BUILD_ROOT

%files
%doc AUTHORS COPYING ChangeLog NEWS README README.nt
%{_bindir}/%{name}
%{_datadir}/games/%{name}
%{_datadir}/gnome/apps/Games/%{name}.desktop
%{_datadir}/locale/*/LC_MESSAGES/*.mo
