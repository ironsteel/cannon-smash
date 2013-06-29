/***********************************************************************
 *	z:/wata/src/a/csmash/loadparts.cpp
 *	$Id: loadparts.cpp,v 1.14 2003/11/19 16:49:31 nan Exp $
 *
 *	Copyright by ESESoft.
 *
 *	Redistribution and use in source and binary forms, with or without
 *	modification, are permitted provided that the following conditions
 *	are met:
 *
 *	Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer. 
 *
 *	Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer
 *	in the documentation and/or other materials provided with the
 *	distribution. 
 *
 *	The name of the author may not be used to endorse or promote
 *	products derived from this software without specific prior written
 *	permission. 
 *
 *	THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 *	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *	ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 *	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***********************************************************************/
#include "ttinc.h"

#include <iostream>
#include <string>
#include <map>
#include <list>
#include <algorithm>

#include "float"
#include "matrix"
#include "affine"

#include "LoadImage.h"
#include "parts.h"
#include "loadparts.h"

// for(;;) namescoping hack.
// VC++ 6 is not compliant with latest ANSI C++ (but VC++7 does).
#if defined(_MSC_VER) && (_MSC_VER <= 1200)
# define for if(0);else for
#endif
#define elif else if

#define BEGIN_ANONYMOUS namespace {
#define END_ANONYMOUS }

/***********************************************************************
 *	Local data and functions
 ***********************************************************************/
BEGIN_ANONYMOUS

template <typename T>
inline bool between(const T& a, const T& x, const T& b) {
    return a <= x && x <= b;
}

template <typename T>
inline const T& clamp(const T& a, const T& x, const T& b) {
    if (a > x) return a;
    elif (b < x) return b;
    else return x;
}

inline bool streq(const char *a, const char *b) {
    return 0 == strcmp(a, b);
}

inline bool strieq(const char *a, const char *b) {
#ifdef _WIN32
    return 0 == _stricmp(a, b);
#else
    return 0 == strcasecmp(a, b);
#endif
}

struct auto_fp
{
    FILE *fp;
    bool needclose;
    inline auto_fp(FILE *fp, bool b = true) : fp(fp), needclose(b) {}
    inline ~auto_fp() { if (needclose && fp) fclose(fp); }
    inline operator FILE*() { return fp; }
    inline bool operator!() const { return !fp; }
};

bool loadAffine4F(const char *str, affine4F *pm)
{
    FILE *fp = fopen(str, "r");
    if (!fp) return false;

    affine4F &m = *pm;
    m = affine4F(1);
    const char *delim = " \t(,);\r\n";
    char line[256];
    int i = 0;
    while (NULL != fgets(line, sizeof(line), fp)) {
	const char *token = strtok(line, delim);
	if (!token) continue;
	do {
	    if (strieq("affine3", token)) continue;
	    Float f = (Float)strtod(token, NULL);
	    int x = i / 3;
	    int y = i % 3;
	    m[x][y] = f;
	    if (12 == ++i) break;
	}
	while ((token = strtok(NULL, delim)));
    }
    fclose(fp);
    if (12 != i) return false;

    return true;
}

END_ANONYMOUS
/***********************************************************************
 *	Class parts
 ***********************************************************************/
// symbol table
#define SYM(A) { parts::sym_##A, #A }
static struct symtab_t {
    parts::symbol_t sym;
    const char *str;
} symtab[] = {
    SYM(null), SYM(load), SYM(create), SYM(polyhedron),
    SYM(anim), SYM(texture), SYM(body),
    { parts::sym_unknown, NULL },
    { parts::sym_unknown, "NanTheBLACK, our guru:-p" }
};
#undef SYM

// parts object store
static parts_map partsmap;

parts::symbol_t parts::getsym(const char *str)
{
    if (!str) return sym_unknown;
    for (symtab_t *p = symtab; p->str; ++p) {
	if (streq(p->str, str)) return p->sym;
    }
    return sym_unknown;
}

const char* parts::sym2str(parts::symbol_t sym)
{
    for (symtab_t *p = symtab; p->str; ++p) {
	if (p->sym == sym) return p->str;
    }
    return "sym_unknown";
}

parts* parts::getobject(const char* name)
{
    const parts_map::iterator i = partsmap.find(name);
    if (partsmap.end() == i) return NULL;
    else return i->second;
}

bool parts::addobject(const char *name, parts* p)
{
    const parts_map::iterator i = partsmap.find(name);
    if (partsmap.end() != i) return false;

    p->name = name;
    partsmap[name] = p;
    return true;
}

bool parts::delobject(const char *name)
{
    parts_map::iterator i = partsmap.find(name);
    if (partsmap.end() == i) return false;
    delete i->second;
    partsmap.erase(i);
    return true;
}

void parts::clearobjects()
{
    partsmap.clear();
}

bool parts::realizeobjects()
{
    bool r = true;
    for (parts_map::iterator i = partsmap.begin(); i != partsmap.end(); ++i) {
	r &= (i->second)->realize();
    }
    return r;
}

void parts::unrealizeobjects()
{
    for (parts_map::iterator i = partsmap.begin(); i != partsmap.end(); ++i) {
	(i->second)->unrealize();
    }
}

bool parts::loadobjects(const char *str)
{
    try {
	loadfile(str);
    }
    catch (const error &e) {
	printf("loadfile failed\n");
	printf(e.what());
	return false;
    }
    return true;
}
    
bool parts::loadfile(const char *str)
{
    auto_fp fp(fopen(str, "r"));
    if (!fp) return false;

    int lineno = 0;
    do {
	const char *delim = " \t\r\n;";
	char line[4096];
	fgets(line, sizeof(line), fp);
	if (feof((FILE*)fp)) break;
	++lineno;
	int l = strlen(line);
	int addline = 0;

	while (l > 0 && (line[l-1] == '\r' || line[l-1] == '\n')) {
	    line[--l] = '\0';
	}

	while ('\\' == line[l-1]) {
            // concat next line(s)
	    int bufsize = clamp((size_t)0, sizeof(line)-l, sizeof(line)-1);
	    fgets(&line[l-2], bufsize, fp);
	    if (feof((FILE*)fp)) break;
	    l = strlen(line);
	    while (l > 0 && (line[l-1] == '\r' || line[l-1] == '\n')) {
		line[--l] = '\0';
	    }
	    ++addline;
	}

	int argc = 0;
	const char *argv[256];
	const char *token = strtok(line, delim);
        const int argcmax = sizeof(argv) / sizeof(const char*);
	if (!token || '#' == *token) continue;
	do {
	    argv[argc++] = token;
	    if (argcmax == argc) {
                throw verror(lineno, "This line has %d or more arguments\n", argcmax);
	    }	
	} while ((token = strtok(NULL, delim)));
	argv[argc] = NULL;
	int optind = 0;

	token = argv[optind++];
	symbol_t sym = getsym(token);
	switch (sym) {
	case sym_load:
	    load_load(lineno, argc, argv, &optind); break;
	case sym_create:
	    load_create(lineno, argc, argv, &optind); break;
	default:
            throw verror(lineno, "error unknown command %s\n", token);
	}
	lineno += addline;
    } while (!ferror((FILE*)fp));

    return true;
}
	
bool parts::load_load(int lineno, int argc, const char *argv[], int* poptind)
{
    int& optind = *poptind;

    const char *token = argv[optind++];
    if (!token) {
	throw verror(lineno, "error type not specified\n");
    }
    symbol_t sym = getsym(token);

    const char *objectname = argv[optind++];
    if (!objectname) {
        throw verror(lineno, "object name is not specified\n");
    }
    
    const char *filename = argv[optind++];
    switch (sym) {
    case sym_texture: {
	texture_parts* object = new texture_parts(objectname);
	if (!addobject(objectname, object)) {
	    delete object;
            throw verror(lineno, "%s is already loaded\n", objectname);
	}
	object->load(filename);
	break;
    } /* texture */
    
    case sym_polyhedron: {
	polyhedron_parts *object = new polyhedron_parts(objectname);
	if (!addobject(objectname, object)) {
	    delete object;
            throw verror(lineno, "%s is already loaded\n", objectname);
	}
	object->load(filename);
	load_polyhedron(lineno, object, argc, argv, &optind);

	break;
    }	/* polyhedron */

    case sym_anim: {
	anim_parts *object = new anim_parts(objectname);
	if (!addobject(objectname, object)) {
	    delete object;
            throw verror(lineno, "%s is already loaded\n", objectname);
	}
	object->load(filename);
	load_anim(lineno, object, argc, argv, &optind);

	break;
    }	 /* anim */

    default:
        throw verror(lineno, "error unknown type(%s) specified\n", token);
    }

    return true;
}

bool parts::load_polyhedron(int lineno, polyhedron_parts *object,
			    int argc, const char* argv[], int *poptind)
{
    int &optind = *poptind;
    const char *option;
    while ((option = argv[optind++])) {
	if ('-' != *option) {
            throw verror(lineno, "unknown option %s\n", option);
	}
	const char *operand = argv[optind++];
	if (!operand) {
            throw verror(lineno, "no operadnd for %s\n", option);
	}
	switch (option[1]) {
	case 'c': {	/* colormap */
	    colormap cmap;
	    if (!cmap.load(operand)) {
                throw verror(lineno, "could not load colormap %s\n", operand);
	    }
	    object->object->cmap = cmap;
	    break;
	}
	case 't': {	/* texture */
	    parts *tex = getobject(operand);
	    if (!tex) {
                throw verror(lineno, "texture %s not loaded\n", operand);
	    }
	    if (!object->assign(tex)) {
                throw verror(lineno, "%s is not assignable\n", operand);
	    }
	    break;
	}
	case 'm': {     /* matrix */
	    affine4F m;
	    if (!loadAffine4F(operand, &m)) {
                throw verror(lineno, "matrix %s cannot be loaded\n", operand);
            }
	    *object->object *= m;

	    break;
	}
	default:
            throw verror(lineno, "unknown option %s\n", option);
	}
    }
    // create normal vectors of polyhedron
    object->object->getNormal();
    return true;
}

bool parts::load_anim(int lineno, anim_parts* object,
		      int argc, const char *argv[], int *poptind)
{
    int& optind = *poptind;
    int i = 0;
    while (const char *name = argv[optind++]) {
	if ('-' == *name) {
	    if (streq("-pre", name) || streq("-post", name)) {
		affine4F m;
		const char *fname = argv[optind++];
		if (!fname || !loadAffine4F(fname, &m)) {
		    throw verror(lineno, "mat %s cannot be loaded\n", fname);
		}
		affineanim &anim = *object->object;
		for (int i = 0; anim.numFrames > i; ++i) {
		    if (streq("-pre", name)) {
			anim.matrices[i] = m * anim.matrices[i];
		    } else {
			anim.matrices[i] *= m;
		    }
		}
	    }
	    else {
		throw verror(lineno, "unknown option %s\n", name);
	    }
	} else {
	    parts *poly = getobject(name);
	    if (!poly) {
                throw verror(lineno, "%s not loaded\n", name);
	    }
	    if (!object->assign(poly)) {
                throw verror(lineno, "%s is not assignable\n", name);
	    }
	    ++i;
	}
    }
    if (!i) {
	printf("%d: %s is empty object\n", lineno, object->name.c_str());
    }
    return true;
}

bool parts::load_create(int lineno, int argc, const char *argv[], int *poptind)
{
    int &optind = *poptind;
    const char *token = argv[optind++];
    if (!token) {
        throw verror(lineno, "object type is not specified\n");
    }
    switch (getsym(token)) {
    case sym_body: {
	const char *objectname = argv[optind++];
	if (!objectname) {
            throw verror(lineno, "object name is not specified\n");
	}
	body_parts *object = new body_parts(objectname);
	if (!addobject(objectname, object)) {
            throw verror(lineno, "%s is already loaded\n", objectname);
	}
	int i = 0;
	while ((token = argv[optind++])) {
	    parts* p = getobject(token);
	    if (!p) {
                throw verror(lineno, "%s is not loaded\n", token);
	    }
	    object->assign(p);
	    ++i;
	}
	if (!i) {
            printf("%d: %s is empty\n", lineno, objectname);
	}
	break;
    }
    default:
        throw verror("type %s cannot be created\n", token);
    }
    return true;
}

/***********************************************************************
 *	Class texture_parts
 ***********************************************************************/
bool texture_parts::load(const char *str)
{
    filename = str;
    return true;
}

void texture_parts::unrealize()
{
    if (object) {
	glDeleteTextures(1, &object);
	object = 0;
    }
}

bool texture_parts::realize()
{
    if (object) return true;

    static int allowedsize[] = {
	64, 128, 130, 256, 512, 0
    };

    ImageData img;
    bool loaded;

    loaded = img.LoadFile(filename.c_str());

    if (!loaded) {
        throw verror("could not load texture %s\n", filename.c_str());
    }
    int width = img.GetWidth();
    int height = img.GetHeight();
    int i, j;
    for (i = 0; 0 != allowedsize[i]; ++i) {
	if (width == allowedsize[i]) break;
    }
    for (j = 0; 0 != allowedsize[i]; ++j) {
	if (height == allowedsize[i]) break;
    }
    if (0 == allowedsize[i] || 0 == allowedsize[j]) {
        throw verror("texture %s has illegal size(%d,%d)\n",
                     filename.c_str(), width, height);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &object);
    glBindTexture(GL_TEXTURE_2D, object);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0,
		 GL_RGBA, GL_UNSIGNED_BYTE, img.GetImage());

    if (0 == object) {
	char buf[256];
	snprintf(buf, sizeof(buf), "texture %s cannot be realized\n",
                 filename.c_str());
	printf(buf);
	throw error(buf);
        return false;
    } else {
        return true;
    }
}

/***********************************************************************
 *	Class polyhedron_parts
 ***********************************************************************/
bool polyhedron_parts::load(const char *str)
{
    object = new polyhedron(str);
    if (!object->points) {
	delete object;
	object = NULL;
        throw verror("polyhedron %s cannot be loaded\n", str);
    }
    return true;
}

bool polyhedron_parts::assign(parts* a)
{
    switch (a->type()) {
    case sym_texture:
	tex = reinterpret_cast<texture_parts*>(a);
	break;
    default:
        throw verror("%s(%s) cannot be assigned to polyhedron(%s)\n",
		 a->name.c_str(), a->typestr(), name.c_str());
    }
    return true;
}

void polyhedron_parts::render() const
{
    float NanTheBLACK[4] = { 0, 0, 0, 1 };
    float ManOfVirtue[4] = { 1, 1, 1, 1 };

    polyhedron &poly = *object;
    if (tex && poly.texcoord && tex->object) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex->object);
//	glColor4fv(ManOfVirtue);
	for (int i = 0; poly.numPolygons > i; ++i) {
	    const polygon &face = poly.getPolygon(i);
	    glBegin(face.glBeginSize());
	    for (int j = 0; face.size > j; ++j) {
		poly.cmap[face.c()].glBind();
		glNormal3fv((float*)&face.rn(j));
		glTexCoord2fv((float*)&face.rst(j));
		glVertex3fv((float*)&face.rv(j));
	    }
	    glEnd();
	}
    } else {
	glDisable(GL_TEXTURE_2D);
	for (int i = 0; poly.numPolygons > i; ++i) {
	    const polygon &face = poly.getPolygon(i);
	    glBegin(face.glBeginSize());
	    for (int j = 0; face.size > j; ++j) {
		poly.cmap[face.c()].glBind();
		glNormal3fv((float*)&face.rn(j));
		glVertex3fv((float*)&face.rv(j));
	    }
	    glEnd();
	}
    }
}

void polyhedron_parts::renderWire(const vector3F& origin) const
{
    polyhedron &poly = *object;

    glBegin(GL_LINES);
    for (int i = 0; poly.numEdges > i; ++i) {
	int p0 = poly.edges[i].p0;
	int p1 = poly.edges[i].p1;
	bool draw = false;
	if (p1 >= 0) {
            // Render if one polygon on the side of edge is visible
            // while other side is not visible.
            vector3F v = poly.points[poly.edges[i].v0] - origin;
	    Float i0 = v * poly.planeNormal[p0];
	    Float i1 = v * poly.planeNormal[p1];
	    if (i0 * i1 <= 0) draw = true;
	} else {
            // This edge has a polygon only on one side.
	    draw = true;
	}
	if (draw) {
	    vector3F &v0 = poly.points[poly.edges[i].v0];
	    vector3F &v1 = poly.points[poly.edges[i].v1];
	    glVertex3fv((float*)&v0);
	    glVertex3fv((float*)&v1);
	}
    }
    glEnd();
}
/***********************************************************************
 *	Class anim_parts
 ***********************************************************************/
bool anim_parts::load(const char *str)
{
    object = new affineanim(str);
    if (!object->matrices) {
        throw verror("could not load anim %s\n", str);
    }
    return true;
}

bool anim_parts::assign(parts* a)
{
    switch (a->type()) {
    case sym_polyhedron:
	poly.push_back(reinterpret_cast<polyhedron_parts*>(a));
	break;
    default:
	throw verror("%s(%s) cannot be assigned to anim(%s)\n",
                     a->name.c_str(), a->typestr(), name.c_str());
    }
    return true;
}

void anim_parts::render(int frame) const
{
    affineanim &anim = *object;
    glPushMatrix();
#ifdef CHIYO
    glTranslatef(0,0,0.1F);	// Her shoes go underground without this:-)
#endif
    glMultMatrixf((float*)&anim[frame]);
    for (std::list<polyhedron_parts*>::const_iterator i = poly.begin();
	 poly.end() != i; ++i) {
	(*i)->render();
    }
    glPopMatrix();
}

void anim_parts::renderWire(int frame) const
{
    affineanim &anim = *object;
    glPushMatrix();
#ifdef CHIYO
    glTranslatef(0,0,0.1F);	// Her shoes go underground without this:-)
#endif
    glMultMatrixf((float*)&anim[frame]);

    affine4F t;
    glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&t);
    vector3F origin = vector3F(0) * ~t;

    for (std::list<polyhedron_parts*>::const_iterator i = poly.begin();
	 poly.end() != i; ++i) {
	(*i)->renderWire(origin);
    }
    glPopMatrix();
}

/***********************************************************************
 *	Class body_parts
 ***********************************************************************/
bool body_parts::assign(parts* a)
{
    switch (a->type()) {
    case sym_anim:
	object.push_back(reinterpret_cast<anim_parts*>(a));
	break;
    default:
        throw verror("%s(%s) cannot be assigned to body (%s)\n",
                     a->name.c_str(), a->typestr(), name.c_str());
    }
    return true;
}

void body_parts::render(int frame) const
{
    for (std::list<anim_parts*>::const_iterator i = object.begin();
	 object.end() != i; ++i) {
	(*i)->render(frame);
    }
}

void body_parts::renderWire(int frame) const
{
    for (std::list<anim_parts*>::const_iterator i = object.begin();
	 object.end() != i; ++i) {
	(*i)->renderWire(frame);
    }
}

/***********************************************************************
 *	END OF loadparts.cpp
 ***********************************************************************/
