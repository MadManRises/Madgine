using System;

using UnityEngine;

namespace Madgine
{

    public class SceneManager
    {

        public static SceneManager instance = new SceneManager();

        SceneManager()
        {
        }

        public void print()
        {
            Debug.Log("Test");
        }

    }

}