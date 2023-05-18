/*
 * driver/clk-r128-pll.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <clk/clk.h>
#include <r128/reg-ccu-aon.h>

struct clk_r128_pll_pdata_t {
	virtual_addr_t virt;
	char * parent;
	int channel;
};

static void clk_r128_pll_set_parent(struct clk_t * clk, const char * pname)
{
}

static const char * clk_r128_pll_get_parent(struct clk_t * clk)
{
	struct clk_r128_pll_pdata_t * pdat = (struct clk_r128_pll_pdata_t *)clk->priv;
	return pdat->parent;
}

static void clk_r128_pll_set_enable(struct clk_t * clk, bool_t enable)
{
}

static bool_t clk_r128_pll_get_enable(struct clk_t * clk)
{
	return TRUE;
}

static void clk_r128_pll_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
}

static u64_t clk_r128_pll_get_rate(struct clk_t * clk, u64_t prate)
{
	struct clk_r128_pll_pdata_t * pdat = (struct clk_r128_pll_pdata_t *)clk->priv;
	u32_t r, n, m;
	u64_t rate = 0;

	switch(pdat->channel)
	{
	case 0:
		r = read32(pdat->virt + CCU_DPLL1_CTRL);
		n = ((r >> 4) & 0xff);
		m = ((r >> 0) & 0xf);
		rate = (u64_t)((prate * n) / m);
		break;

	case 1:
		r = read32(pdat->virt + CCU_DPLL2_CTRL);
		n = ((r >> 4) & 0xff);
		m = ((r >> 0) & 0xf);
		rate = (u64_t)((prate * n) / m);
		break;

	case 2:
		r = read32(pdat->virt + CCU_DPLL3_CTRL);
		n = ((r >> 4) & 0xff);
		m = ((r >> 0) & 0xf);
		rate = (u64_t)((prate * n) / m);
		break;

	default:
		break;
	}

	return rate;
}

static struct device_t * clk_r128_pll_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clk_r128_pll_pdata_t * pdat;
	struct clk_t * clk;
	struct device_t * dev;
	struct dtnode_t o;
	char * parent = dt_read_string(n, "parent", NULL);
	char * name = dt_read_string(n, "name", NULL);
	int channel = dt_read_int(n, "channel", -1);

	if(channel < 0 || channel > 2)
		return NULL;

	if(!parent || !name)
		return NULL;

	if(!search_clk(parent) || search_clk(name))
		return NULL;

	pdat = malloc(sizeof(struct clk_r128_pll_pdata_t));
	if(!pdat)
		return NULL;

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = phys_to_virt(R128_CCU_AON_BASE);
	pdat->parent = strdup(parent);
	pdat->channel = channel;

	clk->name = strdup(name);
	clk->count = 0;
	clk->set_parent = clk_r128_pll_set_parent;
	clk->get_parent = clk_r128_pll_get_parent;
	clk->set_enable = clk_r128_pll_set_enable;
	clk->get_enable = clk_r128_pll_get_enable;
	clk->set_rate = clk_r128_pll_set_rate;
	clk->get_rate = clk_r128_pll_get_rate;
	clk->priv = pdat;

	if(!(dev = register_clk(clk, drv)))
	{
		free(pdat->parent);
		free(clk->name);
		free(clk->priv);
		free(clk);
		return NULL;
	}
	if(dt_read_object(n, "default", &o))
	{
		char * c = clk->name;
		char * p;
		u64_t r;
		int e;

		if((p = dt_read_string(&o, "parent", NULL)) && search_clk(p))
			clk_set_parent(c, p);
		if((r = (u64_t)dt_read_long(&o, "rate", 0)) > 0)
			clk_set_rate(c, r);
		if((e = dt_read_bool(&o, "enable", -1)) != -1)
		{
			if(e > 0)
				clk_enable(c);
			else
				clk_disable(c);
		}
	}
	return dev;
}

static void clk_r128_pll_remove(struct device_t * dev)
{
	struct clk_t * clk = (struct clk_t *)dev->priv;
	struct clk_r128_pll_pdata_t * pdat = (struct clk_r128_pll_pdata_t *)clk->priv;

	if(clk)
	{
		unregister_clk(clk);
		free(pdat->parent);
		free(clk->name);
		free(clk->priv);
		free(clk);
	}
}

static void clk_r128_pll_suspend(struct device_t * dev)
{
}

static void clk_r128_pll_resume(struct device_t * dev)
{
}

static struct driver_t clk_r128_pll = {
	.name		= "clk-r128-pll",
	.probe		= clk_r128_pll_probe,
	.remove		= clk_r128_pll_remove,
	.suspend	= clk_r128_pll_suspend,
	.resume		= clk_r128_pll_resume,
};

static __init void clk_r128_pll_driver_init(void)
{
	register_driver(&clk_r128_pll);
}

static __exit void clk_r128_pll_driver_exit(void)
{
	unregister_driver(&clk_r128_pll);
}

driver_initcall(clk_r128_pll_driver_init);
driver_exitcall(clk_r128_pll_driver_exit);
