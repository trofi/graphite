#ifndef SEGMENT_INCLUDE
#define SEGMENT_INCLUDE

#include <vector>

#include "graphiteng/ISegment.h"
#include "graphiteng/ISlot.h"
#include "Slot.h"
#include "CharInfo.h"
#include "FontFace.h"

class Silf;

class Segment : public ISegment
{
public:
    virtual int length() const { return m_numGlyphs; }
    virtual Position advance() const { return m_advance; }
    virtual Slot & operator[] (int index) { return m_slots[index]; }
    virtual const Slot & operator[] (int index) const { return m_slots[index]; }
    virtual void runGraphite() { if (m_silf) m_face->runGraphite(this, m_silf); };
    virtual void chooseSilf(uint32 script) { m_silf = m_face->chooseSilf(script); }

    Segment(int numSlots, const FontFace *face);
    Segment(const Segment &other);
    ~Segment();
    void appendSlot(int i, int cid, int gid);
    void positionSlots(FontImpl *font);
    void append(const Segment &other);
    void insertSlot(int index) { m_slots.insert(m_slots.begin() + index, Slot()); m_numGlyphs++; }
    void deleteSlot(int index) { m_slots.erase(m_slots.begin() + index); m_numGlyphs--; }

private:
    std::vector<Slot> m_slots;
    std::vector<uint16> m_user;
    int m_numGlyphs;
    CharInfo *m_charinfo;  // character info, one per input character
    int m_numCharinfo;      // size of the array and number of input characters

    const FontFace *m_face;      // Fontface
    const Silf *m_silf;
    Position m_advance;       // whole segment advance
    Rect m_bbox;           // ink box of the segment
};

#ifndef DISABLE_TRACING
extern void logSegment(const ITextSource & textSrc, const ISegment & seg);
#endif

#endif // SEGMENT_INCLUDE
