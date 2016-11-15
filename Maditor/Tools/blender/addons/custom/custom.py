
import os
import re
import shutil
import sys
import struct
import mathutils

from subprocess import call

def addCustomShaders(indent):
    M = ""
    M += indent(3, 'rtshader_system {')
    M += indent(4, 'visibility 0')
    M += indent(3, '}')
    return M

def runCustom(report, export, context):
    srcpath = os.path.dirname(export.filepath)
    report.messages.append(srcpath)

    outputName = os.path.splitext(os.path.basename(export.filepath))[0]

    files = os.listdir(srcpath)

    def r(*s):
        t = r'.*\.('
        b = False
        for a in s:
            if b:
                t += r'|'
            else:
                b = True
            t += a
        t += r')$'

        return [f for f in files if re.match(t, f)]

    def path(s):
        return os.path.join("C:\\Users\\schue\\Desktop\\GitHub\\TT\\Data\\Media", s)


    materialTarget = path("Material")

    for f in r("material"):
        shutil.copyfile(os.path.join(srcpath, f), os.path.join(materialTarget, f))

    meshTarget = path("Meshes")

    for f in r("mesh"):
        shutil.copyfile(os.path.join(srcpath, f), os.path.join(meshTarget, f))

    for f in r("skeleton"):
        shutil.copyfile(os.path.join(srcpath, f), os.path.join(meshTarget, f))

    if not export.EX_SCENE:
        report.messages.append("No Scene Exported!")
        return

    def collectEntities():
        statics = []
        lights = []
        es = {}
        id = 1
        for ob in context.scene.objects:
            if ob.type == "LAMP":
                lights.append(ob)
            elif "Behaviour" in ob:
                id += 1
                es[id] = ob
            elif ob.data:
                statics.append(ob)

        return (es, statics, lights)

    def getMesh(ob):
        if (ob.data):
            return ob.data.name + ".mesh"
        else:
            return ""



    with open(path('Levels\\' + outputName + '.s'), 'wb') as levelOut:
        def writeChar(c):
            levelOut.write(bytes([c]))
        
        def writeInt64(i):
            for _ in range(8):
                levelOut.write(bytes([i & 0xFF]))
                i >>= 8

        def writeInt32(i):
            for _ in range(4):
                levelOut.write(bytes([i & 0xFF]))
                i >>= 8

        def writeNull():
            writeChar(9)

        def writeType(t):
            writeChar(3)
            writeInt32(t)

        def writeEntityType():
            writeType(0)

        def writeLevelType():
            writeType(3)

        def writeId(i):
            writeChar(2)
            writeInt64(i)

        def writeString(s):
            writeChar(6)
            writeInt64(len(s))
            levelOut.write(bytes(s, 'utf-8'))

        def writeInt(i):
            writeChar(3)
            writeInt32(i)

        def writeValue(v):
            if type(v) is str:
                writeString(v)
            elif type(v) is int:
                writeInt(v)
            elif type(v) is float:
                writeFloat(v)
            else:
                raise Exception("Unkown Type: " + str(type(v)))

        def writeFloat(f):
            writeChar(5)
            levelOut.write(struct.pack('f', f))

        def writeVector(v):
            writeChar(7)
            levelOut.write(struct.pack('f'*3, *v))


        (entities, statics, lights) = collectEntities()

        writeString("")

        for s in statics:
            writeString(s.name)
            writeString(getMesh(s))
            v = s.matrix_world.translation
            writeVector([v[0], v[2], -v[1]])
            v = mathutils.Quaternion(s.rotation_euler)
            writeFloat(v[0])
            writeVector([v[1], v[3], v[2]])
        writeNull() #close statics list

        for l in lights:
            v = l.matrix_world.translation
            writeVector([v[0], v[2], -v[1]])
            writeFloat(1.0/l.data.distance)

            if l.data.type == 'POINT':
                writeInt(0)
            elif l.data.type == 'SPOT':
                writeInt(2)
            elif l.data.type == 'SUN':
                writeInt(1)

            writeFloat(l.data.energy)

            if l.data.type in ('SPOT', 'SUN'):
                v = mathutils.Euler.to_matrix(l.rotation_euler)[2]
                writeVector([v[0], v[2], -v[1]])


        writeNull() #close lights list

        writeNull() #close sceneComponentList



        for key, ob in entities.items():
            writeId(key)
            writeEntityType()
            writeString(ob.name)
            writeString(getMesh(ob))
            writeString(ob["Behaviour"])
            getMesh(ob)

        writeId(1) #levelId
        writeLevelType()

        writeNull()

        for key, ob in entities.items():
            writeId(key)
            writeString("")
            for k in ob.keys():
                if k not in ["Behaviour", "_RNA_UI", "cycles"]:
                    writeString(k)
                    writeValue(ob[k])
            writeNull()
            v = ob.location
            writeVector([v[0], v[2], -v[1]])
            v = mathutils.Quaternion(ob.rotation_euler)
            writeFloat(v[0])
            writeVector([v[1], v[3], v[2]])

            v = ob.scale
            writeVector([v[0], v[2], -v[1]])

            #Components
            writeNull()

        writeNull()


    report.messages.append("Done")
