
import morfeusz2

m = morfeusz2.Morfeusz.createInstance()
for mi in m.analyse('Ala ma kota.'):
    print mi.startNode, mi.endNode, mi.orth, mi.lemma, m.getIdResolver().getTag(mi.tagId), m.getIdResolver().getName(mi.nameId), m.getIdResolver().getLabels(mi.labelsId)
