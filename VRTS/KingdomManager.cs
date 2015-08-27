using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;
using System.Linq;

public abstract class KingdomManager : MonoBehaviour
{
    public Castle Cast;
    public GameObject castle;
    public Side side;
    public List<GameObject> troops = new List<GameObject>();
    public List<GameObject> objectives = new List<GameObject>();

    protected int maxTroops { get; set; }
    protected int gold { get; set; }
    protected int food { get; set; }

    bool end = false;



    protected virtual void Update()
    {

        PlayerUpdate();
        EnemyUpdate();

        foreach (GameObject g in troops)
        {
            g.transform.SetParent(gameObject.transform);
        }
        foreach (GameObject g in objectives)
        {
            g.transform.SetParent(gameObject.transform);
        }

        if (Cast.side != side)
        {
            Cast = null;
        }
    }

    private void PlayerUpdate()
    {
        for (int i = 0; i < troops.Count; i++)
        {
            troops[i].GetComponentInChildren<Text>().text = "P" + i;       
        }

        foreach (GameObject obj in objectives)
        {
            Objective objective = obj.GetComponent<Objective>();
            Slider s = obj.GetComponentInChildren<Slider>();
            s.value = objective.health / objective.maxHealth;
            var fill = s.GetComponentsInChildren<Image>().FirstOrDefault(t => t.name == "Fill");
            if (fill != null)
            {
                fill.color = Color.green;
            }
        }
    }


    private void EnemyUpdate()
    {
        if (side.Equals(Side.Enemy))
        {
            for (int i = 0; i < troops.Count; i++)
            {
                Troop troop = troops[i].GetComponent<Troop>();
                troops[i].GetComponentInChildren<Text>().text = "E" + i;
                Slider s = troops[i].GetComponentInChildren<Slider>();
                s.value = troop.health / troop.maxHealth;
                var fill = s.GetComponentsInChildren<Image>().FirstOrDefault(t => t.name == "Fill");
                if (fill != null)
                {
                    fill.color = Color.red;
                }
            }

            foreach (GameObject obj in objectives)
            {
                Objective objective = obj.GetComponent<Objective>();
                Slider s = obj.GetComponentInChildren<Slider>();
                s.value = objective.health / objective.maxHealth;
                var fill = s.GetComponentsInChildren<Image>().FirstOrDefault(t => t.name == "Fill");
                if (fill != null)
                {
                    fill.color = Color.red;
                }
            }
        }
    }


}
