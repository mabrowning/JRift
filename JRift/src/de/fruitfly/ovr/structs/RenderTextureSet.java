package de.fruitfly.ovr.structs;

import java.util.ArrayList;

public class RenderTextureSet {
     public ArrayList<Integer> leftEyeTextureIds = new ArrayList<Integer>();
     public ArrayList<Integer> rightEyeTextureIds = new ArrayList<Integer>();

     public String toString()
     {
         StringBuilder sb = new StringBuilder();
         sb.append("L Texture IDs:").append("\n");
         for (int i = 0; i < leftEyeTextureIds.size(); i++)
             sb.append(" " + i + ": " + leftEyeTextureIds.get(i)).append("\n");
         sb.append("R Texture IDs:").append("\n");
         for (int i = 0; i < rightEyeTextureIds.size(); i++)
             sb.append(" " + i + ": " + rightEyeTextureIds.get(i)).append("\n");
         return sb.toString();
     }
}
