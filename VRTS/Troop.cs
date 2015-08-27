using System.CodeDom.Compiler;
using UnityEngine;
using System.Collections;
using UnityEngine.UI;

public abstract class Troop : MonoBehaviour
{
    public LayerMask blockingLayer;
    public Sprite sprite { get; protected set; }
    protected SpriteRenderer sr;
    //Collision
    public BoxCollider2D boxCollider { get; protected set; }
    protected CircleCollider2D rangeCollider { get; set; }

    public Rigidbody2D rb2D { get; protected set; }

    public GameObject target;

    public Side side { get; set; }

  //Stats
    public float maxHealth;
    public float health;
    public int damage { get; protected set; }
    public float range { get; protected set; }
    public float attackSpeed { get; protected set; }
    public float moveTime { get; protected set; }
    protected float inverseAttackSpeed;
    protected float inverseMoveTime;
    protected float recharge;
    protected bool alive = true;
    private Slider s;
    protected virtual void Start()
    {
      s  = gameObject.GetComponentInChildren<Slider>();

        recharge = 0;

        boxCollider = GetComponent<BoxCollider2D>();

        rangeCollider = GetComponent<CircleCollider2D>();

        rangeCollider.radius = range;

        rb2D = GetComponent<Rigidbody2D>();

        health = maxHealth;
        inverseAttackSpeed = 1f / attackSpeed;
        inverseMoveTime = 1f / moveTime;
    }

    protected virtual void Update()
    {

        s.value = health / maxHealth;

        transform.position += Vector3.zero;
        rangeCollider.transform.position += Vector3.zero;

        if (recharge > 0)
        {
            recharge -= Time.deltaTime;
            //print(recharge);
        }
    }

    void OnTriggerEnter2D(Collider2D other)
    {
        Decide(other.gameObject);
    }

    void OnTriggerStay2D(Collider2D other)
    {
        Decide(other.gameObject);
    }

    public void Decide(GameObject other)
    {
        if (target != null)
        {
            HasTarget();  
        }
        else
        {
            NoTarget(other);
        }
    }


    protected void HasTarget()
    {
        if (rangeCollider.IsTouching(target.GetComponent<BoxCollider2D>()))
        {
            if (gameObject.CompareTag("Player"))
            {
                if (target.CompareTag("Enemy"))
                {
                    Attack(target.gameObject);
                }
                else if (target.CompareTag("Objective"))
                {
                    if (target.GetComponent<Objective>().side.Equals(Side.Enemy))
                    {
                        Attack(target.gameObject);
                    }
                }
            }
            else if (gameObject.CompareTag("Enemy"))
            {
                if (target.CompareTag("Player"))
                {
                    EnemyAttack(target.gameObject);
                }
                else if (target.CompareTag("Objective"))
                {
                    if (target.GetComponent<Objective>().side == Side.Player)
                    {
                        EnemyAttack(target.gameObject);
                    }
                }
            }
        }
    }

    protected void NoTarget(GameObject other)
    {
        if (gameObject.CompareTag("Player"))
        {
            if (other.gameObject.CompareTag("Enemy"))
            {
                if (target == null)
                {
                    target = other.gameObject;
                }
                Attack(other.gameObject);
            }
            else if (other.CompareTag("Objective"))
            {
                if (other.GetComponent<Objective>().side == Side.Enemy)
                {
                    Attack(other.gameObject.gameObject);
                }
                else if (other.GetComponent<Objective>().side == Side.Neutral)
                {
                    other.GetComponent<Objective>().side = Side.Player;
                }
            }
        }
        else if (gameObject.CompareTag("Enemy"))
        {
            if (other.gameObject.CompareTag("Player"))
            {
                if (target == null)
                {
                    target = other.gameObject;
                }
                EnemyAttack(other.gameObject);
            }
            else if (other.CompareTag("Objective"))
            {
                if (other.GetComponent<Objective>().side == Side.Player)
                {
                    if (target == null)
                    {
                        target = other.gameObject;
                    }
                    EnemyAttack(other.gameObject.gameObject);
                }
                else if (other.GetComponent<Objective>().side == Side.Neutral)
                {
                    other.GetComponent<Objective>().side = Side.Enemy;
                }
            }
        }
    }

    protected void TakeDamage(int dmg)
    {
        if (gameObject == null || !alive)
        {
            return;
        }

        health = health - dmg;
        if (health <= 0)
        {
            if (gameObject.CompareTag("Player"))
            {
                GameObject.FindGameObjectWithTag("PM").GetComponent<Player>().troops.Remove(gameObject);
            }
            else if (gameObject.CompareTag("Enemy"))
            {
                GameObject.FindGameObjectWithTag("EM").GetComponent<Enemy>().troops.Remove(gameObject);
            }
            alive = false;
            //gameObject.SetActive(false);
            DestroyObject(gameObject);

        }
    }

    public virtual void EnemyAttack(GameObject component)
    {
        if (recharge <= 0)
        {
            if (component.CompareTag("Player"))
            {
                Troop troop = component.gameObject.GetComponent<Troop>();
                if (rangeCollider.IsTouching(troop.boxCollider))
                {
                    troop.TakeDamage(damage);
                    print("Player: " + troop.health);
                    recharge = attackSpeed;
                }
            }
            else if (component.CompareTag("Objective"))
            {
                Objective objective = component.gameObject.GetComponent<Objective>();
                if (rangeCollider.IsTouching(objective.boxCollider))
                {
                    objective.TakeDamage(damage);
                    print("PO: " + objective.health);
                    recharge = attackSpeed;
                }
            }
        }
    }

    public virtual void Attack(GameObject gameObj)
    {
        if (recharge <= 0)
        {
            if (gameObj.CompareTag("Enemy"))
            {
                Troop troop = gameObj.gameObject.GetComponent<Troop>();
                if (rangeCollider.IsTouching(troop.boxCollider))
                {
                    troop.TakeDamage(damage);
                    print("Enemy: " + troop.health);
                    recharge = attackSpeed;
                }
            }
            else if (gameObj.CompareTag("Objective"))
            {
                Objective objective = gameObj.gameObject.GetComponent<Objective>();
                if (rangeCollider.IsTouching(objective.GetComponent<BoxCollider2D>()))
                {
                    print("EO: " + objective.health);
                    objective.TakeDamage(damage);
                    recharge = attackSpeed;
                }
            }
        }
    }

    public void OnCollisionEnter2D(Collision2D other)
    {

    }

    public virtual void AttemptMove<T>(int xDir, int yDir) where T : Component
    {
        RaycastHit2D hit;
        bool canMove = Move(new Vector2(xDir, yDir), out hit);
        if (!canMove)
        {
            return;
        }
    }

    protected bool Move(Vector2 end, out RaycastHit2D hit)
    {
        bool ret = false;
        Vector2 start = transform.position;
        
      boxCollider.enabled = false;
      hit = Physics2D.Linecast(start, end, blockingLayer);
      boxCollider.enabled = true;

    if (hit.transform == null || !(hit.transform.CompareTag("Wall")))
    {
        StartCoroutine(SmoothMovement(end));
        ret = true;
    }
    else
    {
        print("x: " + hit.transform.position.x + "y: " + hit.transform.position.y );
    }
    return ret;
    }

    protected IEnumerator SmoothMovement(Vector3 end)
    {
        float sqrRemainingDistance = (transform.position - end).sqrMagnitude;

        while (sqrRemainingDistance > float.Epsilon)
        {
            Vector3 newPosition = Vector3.MoveTowards(rb2D.position, end, inverseMoveTime * Time.deltaTime);
            rb2D.MovePosition(newPosition);
            sqrRemainingDistance = (transform.position - end).sqrMagnitude;
            yield return null;
        }
    }
}
