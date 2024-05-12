using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

using System.IO;
using UnityEditor;
using System.Text;
using static TMPro.SpriteAssetUtilities.TexturePacker_JsonArray;
using System.Linq;


public class HierarchicalSceneExporter : MonoBehaviour
{

    private BinaryWriter sceneBinaryWriter=null;
    private BinaryWriter binaryWriter = null; //게임오브젝트 binary file
    private BinaryWriter BoundingWriter = null;
    private List<string> gameObjectNames = new List<string>();
    private List<string> m_pTextureNamesListForCounting = new List<string>();
    private List<string> m_pTextureNamesListForWriting = new List<string>();


    void WriteVector(Vector2 v, BinaryWriter tempWriter)
    {
        tempWriter.Write(v.x);
        tempWriter.Write(v.y);
    }

    void WriteVector(string strHeader, Vector2 v, BinaryWriter tempWriter)
    {
        tempWriter.Write(strHeader);
        WriteVector(v, tempWriter);
    }

    void WriteVector(Vector3 v, BinaryWriter tempWriter)
    {
        tempWriter.Write(v.x);
        tempWriter.Write(v.y);
        tempWriter.Write(v.z);
    }

    void WriteVector(string strHeader, Vector3 v, BinaryWriter tempWriter)
    {
        tempWriter.Write(strHeader);
        WriteVector(v, tempWriter);
    }

    void WriteVector(Vector4 v , BinaryWriter tempWriter)
    {
        tempWriter.Write(v.x);
        tempWriter.Write(v.y);
        tempWriter.Write(v.z);
        tempWriter.Write(v.w);
    }

    void WriteVector(string strHeader, Vector4 v, BinaryWriter tempWriter)
    {
        tempWriter.Write(strHeader);
        WriteVector(v, tempWriter);
    }

    void WriteVector(Quaternion q, BinaryWriter tempWriter)
    {
        tempWriter.Write(q.x);
        tempWriter.Write(q.y);
        tempWriter.Write(q.z);
        tempWriter.Write(q.w);
    }

    void WriteVector(string strHeader, Quaternion q, BinaryWriter tempWriter)
    {
        tempWriter.Write(strHeader);
        WriteVector(q, tempWriter);
    }


    void WriteTransform(string strHeader, Transform current, BinaryWriter tempWriter)
    {
        tempWriter.Write(strHeader);
        WriteVector(current.localPosition, tempWriter);
        WriteVector(current.localEulerAngles, tempWriter);
        WriteVector(current.localScale, tempWriter);
        WriteVector(current.localRotation, tempWriter);
    }


    void WriteWorldMatrix(string strHeader, Transform current, BinaryWriter tempWriter)
    {
        tempWriter.Write(strHeader);
        Matrix4x4 matrix = Matrix4x4.identity;
        matrix = current.localToWorldMatrix;
        WriteMatrix(matrix,tempWriter);
    }


    void WriteMatrix(Matrix4x4 matrix,BinaryWriter tempWriter)
    {
        tempWriter.Write(matrix.m00);
        tempWriter.Write(matrix.m10);
        tempWriter.Write(matrix.m20);
        tempWriter.Write(matrix.m30);
        tempWriter.Write(matrix.m01);
        tempWriter.Write(matrix.m11);
        tempWriter.Write(matrix.m21);
        tempWriter.Write(matrix.m31);
        tempWriter.Write(matrix.m02);
        tempWriter.Write(matrix.m12);
        tempWriter.Write(matrix.m22);
        tempWriter.Write(matrix.m32);
        tempWriter.Write(matrix.m03);
        tempWriter.Write(matrix.m13);
        tempWriter.Write(matrix.m23);
        tempWriter.Write(matrix.m33);
    }

  
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FindTextureByName(List<string> pTextureNamesList, Texture texture)
    {
        if (texture)
        {
            string strTextureName = string.Copy(texture.name).Replace(" ", "_");
            for (int i = 0; i < pTextureNamesList.Count; i++)
            {
                if (pTextureNamesList.Contains(strTextureName)) return (true);
            }
            pTextureNamesList.Add(strTextureName);
            return (false);
        }
        else
        {
            return (true);
        }
    }


    void WriteTextureName(string strHeader, Texture texture)
    {
        binaryWriter.Write(strHeader);
        if (texture)
        {
            if (FindTextureByName(m_pTextureNamesListForWriting, texture))
            {
                binaryWriter.Write("@" + string.Copy(texture.name).Replace(" ", "_"));
            }
            else
            {
                binaryWriter.Write(string.Copy(texture.name).Replace(" ", "_"));
            }
        }
        else
        {
            binaryWriter.Write("null");
        }
    }

    void WriteString(string strToWrite)
    {
        binaryWriter.Write(strToWrite);
    }

    void WriteString(string strHeader, string strToWrite)
    {
        binaryWriter.Write(strHeader);
        binaryWriter.Write(strToWrite);
    }

    void WriteInteger(string strHeader, int i)
    {
        binaryWriter.Write(strHeader);
        binaryWriter.Write(i);
    }

    void WriteFloat(string strHeader, float f)
    {
        binaryWriter.Write(strHeader);
        binaryWriter.Write(f);
    }


    void WriteObjectName(string strHeader, int i, Object obj)
    {
        binaryWriter.Write(strHeader);
        binaryWriter.Write(i);
        binaryWriter.Write((obj) ? string.Copy(obj.name).Replace(" ", "_") : "null");
    }



    void WriteBoundingBox(string strHeader, Bounds bounds)
    {
        binaryWriter.Write(strHeader);
        WriteVector(bounds.center, binaryWriter);
        WriteVector(bounds.extents, binaryWriter);
    }



    void WriteColor(Color c)
    {
        binaryWriter.Write(c.r);
        binaryWriter.Write(c.g);
        binaryWriter.Write(c.b);
        binaryWriter.Write(c.a);
    }

    void WriteColor(string strHeader, Color c)
    {
        binaryWriter.Write(strHeader);
        WriteColor(c);
    }

    void WriteTextureCoord(Vector2 uv)
    {
        binaryWriter.Write(uv.x);
        binaryWriter.Write(1.0f - uv.y);
    }



    void WriteVectors(string strHeader, Vector2[] vectors)
    {
        binaryWriter.Write(strHeader);
        binaryWriter.Write(vectors.Length);
        if (vectors.Length > 0) foreach (Vector2 v in vectors) WriteVector(v,binaryWriter);
    }


    void WriteVectors(string strHeader, Vector3[] vectors)
    {
        binaryWriter.Write(strHeader);
        binaryWriter.Write(vectors.Length);
        if (vectors.Length > 0) foreach (Vector3 v in vectors) WriteVector(v,binaryWriter);
    }

    void WriteVectors(string strHeader, Vector4[] vectors)
    {
        binaryWriter.Write(strHeader);
        binaryWriter.Write(vectors.Length);
        if (vectors.Length > 0) foreach (Vector4 v in vectors) WriteVector(v, binaryWriter);
    }

    void WriteColors(string strHeader, Color[] colors)
    {
        binaryWriter.Write(strHeader);
        binaryWriter.Write(colors.Length);
        if (colors.Length > 0) foreach (Color c in colors) WriteColor(c);
    }

    void WriteTextureCoords(string strHeader, Vector2[] uvs)
    {
        binaryWriter.Write(strHeader);
        binaryWriter.Write(uvs.Length);
        if (uvs.Length > 0) foreach (Vector2 uv in uvs) WriteTextureCoord(uv);
    }

    void WriteIntegers(int[] pIntegers)
    {
        binaryWriter.Write(pIntegers.Length);
        foreach (int i in pIntegers) binaryWriter.Write(i);
    }

    void WriteIntegers(string strHeader, int[] pIntegers)
    {
        binaryWriter.Write(strHeader);
        binaryWriter.Write(pIntegers.Length);
        if (pIntegers.Length > 0) foreach (int i in pIntegers) binaryWriter.Write(i);
    }

    void WriteIntegers(string strHeader, int n, int[] pIntegers)
    {
        binaryWriter.Write(strHeader);
        binaryWriter.Write(n);
        binaryWriter.Write(pIntegers.Length);
        if (pIntegers.Length > 0) foreach (int i in pIntegers) binaryWriter.Write(i);
    }






    bool FindObjectByName(string strObjectName)
    {
        for (int i = 0; i < gameObjectNames.Count; i++)
        {
            if (gameObjectNames.Contains(strObjectName)) return (true);
        }
        gameObjectNames.Add(strObjectName);
        return (false);
    }





    void WriteMeshInfo(Mesh mesh)
    {
        WriteObjectName("<Mesh>:", mesh.vertexCount, mesh);

        WriteBoundingBox("<Bounds>:", mesh.bounds);

        WriteVectors("<Positions>:", mesh.vertices);
        WriteColors("<Colors>:", mesh.colors);
        WriteTextureCoords("<TextureCoords0>:", mesh.uv);
        WriteTextureCoords("<TextureCoords1>:", mesh.uv2);
        WriteVectors("<Normals>:", mesh.normals);

        if ((mesh.normals.Length > 0) && (mesh.tangents.Length > 0))
        {
            Vector3[] tangents = new Vector3[mesh.tangents.Length];
            Vector3[] biTangents = new Vector3[mesh.tangents.Length];
            for (int i = 0; i < mesh.tangents.Length; i++)
            {
                tangents[i] = new Vector3(mesh.tangents[i].x, mesh.tangents[i].y, mesh.tangents[i].z);
                biTangents[i] = Vector3.Normalize(Vector3.Cross(mesh.normals[i], tangents[i])) * mesh.tangents[i].w;
            }

            WriteVectors("<Tangents>:", tangents);
            WriteVectors("<BiTangents>:", biTangents);
        }

        //        WriteIntegers("<Indices>:", mesh.triangles);

        WriteInteger("<SubMeshes>:", mesh.subMeshCount);
        if (mesh.subMeshCount > 0)
        {
            for (int i = 0; i < mesh.subMeshCount; i++)
            {
                int[] subindicies = mesh.GetTriangles(i);
                WriteIntegers("<SubMesh>:", i, subindicies);
            }
        }

        WriteString("</Mesh>");
    }

    void WriteMaterialsInfo(Material[] materials)
    {
        WriteInteger("<Materials>:", materials.Length);
        for (int i = 0; i < materials.Length; i++)
        {
            WriteInteger("<Material>:", i);

            if (materials[i].HasProperty("_Color"))
            {
                Color albedo = materials[i].GetColor("_Color");
                WriteColor("<AlbedoColor>:", albedo);
            }
            if (materials[i].HasProperty("_EmissionColor"))
            {
                Color emission = materials[i].GetColor("_EmissionColor");
                WriteColor("<EmissiveColor>:", emission);
            }
            if (materials[i].HasProperty("_SpecColor"))
            {
                Color specular = materials[i].GetColor("_SpecColor");
                WriteColor("<SpecularColor>:", specular);
            }
            if (materials[i].HasProperty("_Glossiness"))
            {
                WriteFloat("<Glossiness>:", materials[i].GetFloat("_Glossiness"));
            }
            if (materials[i].HasProperty("_Smoothness"))
            {
                WriteFloat("<Smoothness>:", materials[i].GetFloat("_Smoothness"));
            }
            if (materials[i].HasProperty("_Metallic"))
            {
                WriteFloat("<Metallic>:", materials[i].GetFloat("_Metallic"));
            }
            if (materials[i].HasProperty("_SpecularHighlights"))
            {
                WriteFloat("<SpecularHighlight>:", materials[i].GetFloat("_SpecularHighlights"));
            }
            if (materials[i].HasProperty("_GlossyReflections"))
            {
                WriteFloat("<GlossyReflection>:", materials[i].GetFloat("_GlossyReflections"));
            }
            if (materials[i].HasProperty("_MainTex"))
            {
                Texture mainAlbedoMap = materials[i].GetTexture("_MainTex");
                WriteTextureName("<AlbedoMap>:", mainAlbedoMap);
            }
            if (materials[i].HasProperty("_SpecGlossMap"))
            {
                Texture specularcMap = materials[i].GetTexture("_SpecGlossMap");
                WriteTextureName("<SpecularMap>:", specularcMap);
            }
            if (materials[i].HasProperty("_MetallicGlossMap"))
            {
                Texture metallicMap = materials[i].GetTexture("_MetallicGlossMap");
                WriteTextureName("<MetallicMap>:", metallicMap);
            }
            if (materials[i].HasProperty("_BumpMap"))
            {
                Texture bumpMap = materials[i].GetTexture("_BumpMap");
                WriteTextureName("<NormalMap>:", bumpMap);
            }
            if (materials[i].HasProperty("_EmissionMap"))
            {
                Texture emissionMap = materials[i].GetTexture("_EmissionMap");
                WriteTextureName("<EmissionMap>:", emissionMap);
            }
            if (materials[i].HasProperty("_DetailAlbedoMap"))
            {
                Texture detailAlbedoMap = materials[i].GetTexture("_DetailAlbedoMap");
                WriteTextureName("<DetailAlbedoMap>:", detailAlbedoMap);
            }
            if (materials[i].HasProperty("_DetailNormalMap"))
            {
                Texture detailNormalMap = materials[i].GetTexture("_DetailNormalMap");
                WriteTextureName("<DetailNormalMap>:", detailNormalMap);
            }
        }
        WriteString("</Materials>");
    }




    void WriteFrameInfo(Transform current)
    {
        MeshFilter meshFilter = current.gameObject.GetComponent<MeshFilter>();
        MeshRenderer renderer = current.gameObject.GetComponent<MeshRenderer>();
        if (meshFilter && renderer)
        {
            string strToRemove = " (";
            int i = current.name.IndexOf(strToRemove);
            string strObjectName = (i > 0) ? string.Copy(current.name).Remove(i) : string.Copy(current.name);

            string strFilteredObjectName = strObjectName.Replace(' ', '_');


            sceneBinaryWriter.Write("<GameObject>:");
            sceneBinaryWriter.Write(strFilteredObjectName);

            WriteTransform("<Transform>:", current, sceneBinaryWriter);
            WriteWorldMatrix("<TransformMatrix>:", current, sceneBinaryWriter);


            if (FindObjectByName(current.gameObject.name) == false)
            {
                binaryWriter = new BinaryWriter(File.Open(string.Copy(current.gameObject.name).Replace(" ", "_") + ".bin", FileMode.Create));

                Mesh mesh = meshFilter.mesh;
                if (mesh) WriteMeshInfo(mesh);

                Material[] materials = renderer.materials;
                if (materials.Length > 0) WriteMaterialsInfo(materials);
            }
        }
    }


    Transform[] GetLOD0Children(Transform parent)
    {
        // LINQ를 사용하여 이름에 "LOD0"이 들어간 자식들만 필터링합니다.
        return parent.OfType<Transform>().Where(child => child.name.Contains("LOD0")).ToArray();
    }

    void WriteFrameHierarchyInfo(Transform child)
    {
        WriteFrameInfo(child);

        if (child.childCount > 0)
        {
            // 현재 GameObject의 자식 중에서 이름에 "LOD0"이 들어간 오브젝트들을 찾습니다.
            Transform[] lod0Children = GetLOD0Children(child);

            foreach (Transform lod0child in lod0Children)
            {
                WriteFrameHierarchyInfo(lod0child);
            }
           
        }
   }


    int GetRootGameObjectsCount()
    {
        int nGameObjects = 0;

        Scene scene = transform.gameObject.scene;
        GameObject[] rootGameObjects = scene.GetRootGameObjects();
        
        return (rootGameObjects.Length);
    }

    void WriteBounding(Transform child)
    {
        BoundingWriter = new BinaryWriter(File.Open("boundinginfo.bin", FileMode.Create));


        if (child.childCount > 0)
        {
            print(child.childCount);
            int BoundCount = 0 ;
            //메쉬 갯수 세기
             for (int i = 0; i < child.childCount; i++)
             {
                Transform BoundingInfo = child.GetChild(i);
                BoundCount += BoundingInfo.childCount;
             }
             //Bounding자식들 더해주기
            BoundCount += child.childCount;
                BoundingWriter.Write(BoundCount);
            

            for (int i  = 0;  i < child.childCount; i++)
            {
                Transform BoundingInfo = child.GetChild(i);

                Matrix4x4 matrix = Matrix4x4.identity;
                matrix = BoundingInfo.localToWorldMatrix;
                WriteMatrix(matrix, BoundingWriter);
                WriteVector(BoundingInfo.GetComponent<BoxCollider>().size , BoundingWriter);
               
                for (int j = 0; j < BoundingInfo.childCount; j++)
                {
                    Transform childeBoundingInfo = BoundingInfo.GetChild(j);

                    matrix = Matrix4x4.identity;
                    matrix = childeBoundingInfo.localToWorldMatrix;
                    WriteMatrix(matrix, BoundingWriter);
                    WriteVector(childeBoundingInfo.GetComponent<BoxCollider>().size, BoundingWriter);

                }
              




            }


        }
        BoundingWriter.Flush();
        BoundingWriter.Close();

    }


    void Start()
    {
        sceneBinaryWriter = new BinaryWriter(File.Open("Scene.bin", FileMode.Create));

        int nGameObjects = GetRootGameObjectsCount();
        sceneBinaryWriter.Write("<GameObjects>:");
        sceneBinaryWriter.Write(nGameObjects);

        Scene scene = transform.gameObject.scene;
        GameObject[] rootGameObjects = scene.GetRootGameObjects();


        foreach (GameObject gameObject in rootGameObjects)
        {
           
            if (gameObject.name == "Bounding")
            {
          
                WriteBounding(gameObject.transform);
            }
            else
            {
               // WriteFrameHierarchyInfo(gameObject.transform);
            }
           // print("ㅎㅇ \n");
        }
        sceneBinaryWriter.Write("</GameObjects>");

        sceneBinaryWriter.Flush();
        sceneBinaryWriter.Close();

        print("Scene Write Completed");
    }


}
