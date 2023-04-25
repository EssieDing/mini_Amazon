from django.contrib.auth.decorators import login_required
from django.http import JsonResponse
from django.shortcuts import render, redirect, get_object_or_404
from django.urls import reverse
from cart.forms import CartAddProductForm

from .models import *
from orders.models import Order


# Create your views here.
def home(request):
    products = Product.objects.all().order_by("id")
    context = {"products": products}
    return render(request,"amazon/home.html", context)


# def product_detail(request, product_id):
#     product = Product.objects.get(pk=product_id)
#     context = {}
#     if request.method == "POST":
#         if not request.user.is_authenticated:
#             return redirect(reverse("login"))
#         cnt = int(request.POST["count"])
#         if request.POST["action"] == "buy":
#             # do nothing for now
#             order = Order(owner=request.user, product=product, product_cnt=cnt)
#             order.save()
#         else:
#             try:
#                 # try to get an existing order
#                 exist_order = Order.objects.get(owner=request.user, product=product)
#                 exist_order.product_cnt += cnt
#                 exist_order.save()
#             except Order.DoesNotExist:
#                 # create a new order
#                 order = Order(owner=request.user, product=product, product_cnt=cnt)
#                 order.save()
#         return render(request, "amazon/success.html", context)
#     else:
#         context["product"] = product
#         return render(request, "amazon/product_detail.html", context)


@login_required
def shop_cart(request):
    orders = Order.objects.filter(owner=request.user)
    if request.method == 'POST':
        operation = request.POST["operation"]
        # user delete some order
        if operation == "delete":
            oid = request.POST["order_id"]
            orders.get(pk=oid).delete()
        elif operation == "checkout":
            # get all checked orders
            checked_orders = request.POST.getlist("checked_orders")
            print(checked_orders)
            # will only create a new package when at least one order is chosen
            if len(checked_orders) > 0:
                return render(request, "amazon/success.html")
        # api for calculating the total price
        elif operation == "cal_total" and request.is_ajax():
            checked_orders = request.POST.getlist("checked_orders")
            total = 0.0
            for o in checked_orders:
                total += orders.get(pk=o).total()
            return JsonResponse({"total_cart": ("%.2f" % total)})
    total = 0
    for o in orders:
        total += o.total()
    context = {"orders": orders, "total": total}
    return render(request, "amazon/shopping_cart.html", context)


@login_required
def change_cnt(request):
    if request.is_ajax() and request.method == "POST":
        order_id = request.POST["order_id"]
        operation = request.POST["operation"]
        total_cart = float(request.POST["total_cart"])
        order = Order.objects.get(pk=order_id)
        # lower and upper limit --- 1 ~ 99
        if operation == "add" and order.product_cnt < 99:
            order.product_cnt += 1
            order.save()
            total_cart += order.product.price
        elif operation == "minus" and order.product_cnt > 1:
            order.product_cnt -= 1
            order.save()
            total_cart -= order.product.price
        data = {
            # latest count
            "cnt": order.product_cnt,
            # total price for the order
            "total_order": ("%.2f" % order.total()),
            # total price for all
            "total_cart": ("%.2f" % total_cart)
        }
        return JsonResponse(data)
    return JsonResponse({})


def product_list(request, category_slug=None):
    category = None
    categories = Category.objects.all()
    products = Product.objects.all().order_by("id")
    if category_slug:
        category = get_object_or_404(Category, slug=category_slug)
        products = products.filter(category=category)
    return render(request,
                  'amazon/list.html',
                  {'category': category,
                   'categories': categories,
                   'products': products})

def product_detail(request, id, slug):
    product = get_object_or_404(Product,
                                id=id,
                                slug=slug)
    cart_product_form = CartAddProductForm()
    return render(request,
                  'amazon/detail.html',
                  {'product': product,
                   'cart_product_form': cart_product_form})
